/*
    FLAM3 - cosmic recursive fractal flames
    Copyright (C) 1992-2009 Spotworks LLC

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "emscripten.h"

#ifdef WIN32
#define WINVER 0x0500
#include <windows.h>
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

#include <limits.h>

#include "private.h"
#include "img.h"
#include "isaacs.h"

int calc_nstrips(flam3_frame *spec)
{
   double mem_required;
   double mem_available;
   int nstrips, ninc;
   char *testmalloc;
#ifdef WIN32
   MEMORYSTATUS stat;
   stat.dwLength = sizeof(stat);
   GlobalMemoryStatus(&stat); // may want to go to GlobalMemoryStatusEx eventually
   mem_available = (double)stat.dwTotalPhys;
//  fprintf(stderr,"%lu bytes free memory...\n",(size_t)stat.dwAvailPhys);
//  if (mem_available > 1e9) mem_available = 1e9;
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
   mem_available =
       (double)sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
#elif defined __APPLE__
#ifdef __LP64__
   long physmem;
   size_t len = sizeof(physmem);
   static int mib[2] = {CTL_HW, HW_MEMSIZE};
#else
   unsigned int physmem;
   size_t len = sizeof(physmem);
   static int mib[2] = {CTL_HW, HW_PHYSMEM};
#endif
   if (sysctl(mib, 2, &physmem, &len, NULL, 0) == 0 && len == sizeof(physmem))
   {
      mem_available = (double)physmem;
   }
   else
   {
      fprintf(stderr, "warning: unable to determine physical memory.n");
      mem_available = 2e9;
   }
#else
   fprintf(stderr, "warning: unable to determine physical memory.\n");
   mem_available = 2e9;
#endif
#if 0
  fprintf(stderr,"available phyical memory is %lu\n",
	  (unsigned long)mem_available);
#endif
   mem_available *= 0.8;
   if (getenv("use_mem"))
   {
      mem_available = atof(getenv("use_mem"));
   }
   mem_required = flam3_render_memory_required(spec);
   if (mem_available >= mem_required)
      return 1;
   nstrips = (int)ceil(mem_required / mem_available);

   if (0)
   {
      /* Attempt to malloc a strip, and if it fails, try adding additional strips */
      ninc = -1;
      testmalloc = NULL;
      while (NULL == testmalloc && ninc < 3)
      {
         ninc++;
         testmalloc = (char *)malloc((int)ceil(mem_required / (nstrips + ninc)));
      }
      if (NULL == testmalloc)
      {
         fprintf(stderr, "Unable to allocate memory for render.  Please close some running programs and try to render again.\n");
         exit(1);
      }
      else
      {
         free(testmalloc);
         nstrips = nstrips + ninc;
      }
   }

   return nstrips;
}

int print_progress(void *foo, double fraction, int stage, double eta)
{
   fprintf(stderr, "stage=%s progress=%g eta=%g\n", stage ? "filtering" : "chaos", fraction, eta);
   return 0;
}

EMSCRIPTEN_KEEPALIVE
int test()
{
   return 41;
}

flam3_frame f;
void *image = NULL;
size_t this_size, last_size = -1;

double qs = 1.0;

int main(int argc, char **argv)
{
   char *ai;
   flam3_genome *cps;
   int ncps;
   int i;
   FILE *fp;
   char fname[256];
   double imgmem;
   unsigned int strip;
   double center_y, center_base;
   unsigned int nstrips = 1;
   randctx savectx;
   char *prefix = args("prefix", "");
   char *out = args("out", NULL);
   char *format = "jpg"; //getenv("format");
   int verbose = 0;     //argi("verbose", 1); //set verbose to 0
   int bits = argi("bits", 33);
   int bpc = argi("bpc", 8);
   int transparency = argi("transparency", 0);
   // char *inf = getenv("in");
   char *inf = "test.flam3"; // replace getting file input with just passing file name we create on js side
   double ss = argf("ss", 1.0);
   double pixel_aspect = argf("pixel_aspect", 1.0);
   int sub_batch_size = argi("sub_batch_size", 10000);
   int name_enable = argi("name_enable", 0);
   int num_threads = 1; //argi("nthreads",0); //force set to 1
   int earlyclip = argi("earlyclip", 0);
   FILE *in;
   double zoom_scale;
   unsigned int channels;
   long start_time = (long)time(0);
   flam3_img_comments fpc;
   stat_struct stats;
   char numiter_string[64];
   char badval_string[64];
   char rtime_string[64];

   // // #ifdef WIN32

   // //    char *slashloc;
   // //    char exepath[256];
   // //    char palpath[256];
   // //    memset(exepath,0,256);
   // //    memset(palpath,0,256);

   // //     slashloc = strrchr(argv[0],'\\');
   // // 	if (NULL==slashloc) {
   // // 	   sprintf(palpath,"flam3_palettes=flam3-palettes.xml");
   // // 	} else {
   // //        strncpy(exepath,argv[0],slashloc-argv[0]+1);
   // // 	   sprintf(palpath,"flam3_palettes=%sflam3-palettes.xml",exepath);
   // // 	}
   // // 	putenv(palpath);

   // // #endif

   if (1 != argc)
   {
      docstring();
      exit(0);
   }

   /* Init random number generators */
   flam3_init_frame(&f);
   flam3_srandom();

   /* Set the number of threads */
   if (num_threads == 0)
   {
      num_threads = flam3_count_nthreads();
      if (verbose > 1)
         fprintf(stderr, "Automatically detected %d core(s)...\n", num_threads);
   }
   else
   {
      if (verbose)
         fprintf(stderr, "Manually specified %d thread(s)...\n", num_threads);
   }

   // checks getenv("format") for format and default to png
   if (NULL == format)
      format = "png";
   if (strcmp(format, "jpg") &&
       strcmp(format, "ppm") &&
       strcmp(format, "png"))
   {
      fprintf(stderr,
              "format must be either jpg, ppm, or png, not %s.\n",
              format);
      exit(1);
   }

   // Gets the channel
   channels = strcmp(format, "png") ? 3 : 4;

   /* Check for 16-bit-per-channel processing */
   if ((16 == bpc) && (strcmp(format, "png") != 0))
   {
      fprintf(stderr, "Support for 16 bpc images is only present for the png format.\n");
      exit(1);
   }
   else if (bpc != 8 && bpc != 16)
   {
      fprintf(stderr, "Unexpected bpc specified (%d)\n", bpc);
      exit(1);
   }

   if (pixel_aspect <= 0.0)
   {
      fprintf(stderr, "pixel aspect ratio must be positive, not %g.\n",
              pixel_aspect);
      exit(1);
   }

   // If there is a file found from getenv("in") then open that and read, else read stdin
   if (inf)
      in = fopen(inf, "rb");
   else
      in = stdin;
   if (NULL == in)
   {
      perror(inf);
      exit(1);
   }

   // "in" is a FILE object
   // "cps" is a flam3_genome struct
   // No mention in this file of flam3_defaults_on
   // ncps is the number of fractals being rendered
   cps = flam3_parse_from_file(in, inf, flam3_defaults_on, &ncps);

   // fprintf(stderr, "\n Start palett print \n");
   // fprintf(stderr, "bits: %d\n", bits);

   if (NULL == cps)
   {
      fprintf(stderr, "error reading genomes from file\n");
      exit(1);
   }

   // If we had inf FILE then close it now that we're done reading
   if (inf)
      fclose(in);

   // This seems like setup for the flam3_gnome object
   for (i = 0; i < ncps; i++)
   {
      /* Force ntemporal_samples to 1 for -render */
      cps[i].ntemporal_samples = 1;
      cps[i].sample_density *= qs;
      cps[i].height = (int)(cps[i].height * ss);
      cps[i].width = (int)(cps[i].width * ss);
      cps[i].pixels_per_unit *= ss;
      if (cps[i].height <= 0 || cps[i].width <= 0)
      {
         fprintf(stderr, "output image has dimension <=0, aborting.\n");
         exit(1);
      }
   }

   // Just a warning for multiple images
   if (out && (ncps > 1))
   {
      fprintf(stderr, "hqi-flame: warning: writing multiple images "
                      "to one file.  all but last will be lost.\n");
   }

   // fprintf(stderr, "number of fractals to render: %d\n", ncps);
   // fprintf(stderr, "bits: %d\n", bits);
   // fprintf(stderr, "pixel aspect ration %f\n", pixel_aspect);
   // fprintf(stderr, "num threads: %d\n", num_threads); //Note that this is 12, which might have been gotten from configure step on local machine (as we do not have this threaded built)
                                                      // Iterate through the fractals being generated
   for (i = 0; i < ncps; i++)
   {
      int real_height;

      if (verbose && ncps > 1)
      {
         fprintf(stderr, "flame = %d/%d \n", i + 1, ncps);
      }
      // f is a flame3_frame object
      //      f.temporal_filter_radius = 0.0; //this was commented out in original
      f.genomes = &cps[i]; //grab the genomem from cps, which was gotten from parsing input file or stdin
      f.ngenomes = 1;
      f.verbose = verbose;
      f.bits = bits; //gotten from argi("bits", 33); which i assume means defaults to 33
      f.time = 0.0;
      f.pixel_aspect_ratio = pixel_aspect;
      f.progress = 0; //print_progress;
      f.nthreads = num_threads;
      f.earlyclip = earlyclip;
      f.sub_batch_size = sub_batch_size;

      if (16 == bpc)
         f.bytes_per_channel = 2;
      else
         f.bytes_per_channel = 1;

      if (getenv("nstrips"))
      {
         nstrips = atoi(getenv("nstrips"));
      }
      else
      {
         nstrips = calc_nstrips(&f);
      }
      // fprintf(stderr, "number of strips: %d\n", nstrips);

      if (nstrips > cps[i].height)
      {
         fprintf(stderr, "cannot have more strips than rows but %d>%d.\n",
                 nstrips, cps[i].height);
         exit(1);
      }

      // setup the amount of memory needed based on channels (3 for png, 4 for jpg (though might be other way around)), width, heigh, and bytes_per_channel (channel defaults to 1 unless set to 2 by argi("bpc",8);)
      imgmem = (double)channels * (double)cps[i].width * (double)cps[i].height * f.bytes_per_channel;
      // fprintf(stderr, "\nimgmem: %f\n", imgmem);

      if (imgmem > ULONG_MAX)
      {
         fprintf(stderr, "Image size > ULONG_MAX.  Aborting.\n");
         exit(1);
      }

      this_size = (size_t)channels * (size_t)cps[i].width * (size_t)cps[i].height * f.bytes_per_channel;
      // fprintf(stderr, "this_size: %zu\n", this_size);

      // I bet this checks if you can use the same block of memory for this picture as you did for the last and allocates new block if not so
      if (this_size != last_size)
      {
         // fprintf(stderr, "FYI this_size != last_size, meaning your fractals differ in size");

         if (last_size != -1)
         {
            free(image);
            fprintf(stderr, "just freed image because last_size != -1");
         }
         last_size = this_size;
         image = (void *)calloc(this_size, sizeof(char));
         if (NULL == image)
         {
            fprintf(stderr, "Error allocating memory for image.  Aborting\n");
            exit(1);
         }
      }
      else
      {
         memset(image, 0, this_size); //sets a block of memory starting at image and of size this_size
      }

      cps[i].sample_density *= nstrips;
      real_height = cps[i].height;
      cps[i].height = (int)ceil(cps[i].height / (double)nstrips);
      center_y = cps[i].center[1];
      zoom_scale = pow(2.0, cps[i].zoom);
      center_base = center_y - ((nstrips - 1) * cps[i].height) /
                                   (2 * cps[i].pixels_per_unit * zoom_scale);

      /* Copy off random context to use for each strip */
      memcpy(&savectx, &f.rc, sizeof(randctx));

      // iterate through strips, though note that nstrips is 1 in test cases
      for (strip = 0; strip < nstrips; strip++)
      {
         size_t ssoff = (size_t)cps[i].height * strip * cps[i].width * channels * f.bytes_per_channel;
         void *strip_start = image + ssoff; // When only 1 strip, this is the same pointer as image because ssoff is 0
         cps[i].center[1] = center_base + cps[i].height * (double)strip / (cps[i].pixels_per_unit * zoom_scale);

         if ((cps[i].height * (strip + 1)) > real_height)
         {
            int oh = cps[i].height;
            cps[i].height = real_height - oh * strip;
            cps[i].center[1] -=
                (oh - cps[i].height) * 0.5 /
                (cps[i].pixels_per_unit * zoom_scale);
         }

         /* Use the same random context for each strip */
         memcpy(&f.rc, &savectx, sizeof(randctx));

         if (verbose && nstrips > 1)
         {
            fprintf(stderr, "strip = %d/%d\n", strip + 1, nstrips);
         }
         if (verbose && (1 == nstrips) && (ncps > 1))
         {
            fprintf(stderr, "\n");
         }
         cps[i].ntemporal_samples = 1;

         // lets add some print statements!
         // fprintf(stderr, "\n Start debug info \n\n");

         // fprintf(stderr, "nstrips : %d \n", nstrips);

         // fprintf(stderr, "genomes.num_xforms : %d\n", f.genomes->num_xforms);
         // fprintf(stderr, "genomes.genome_index : %d\n", f.genomes->genome_index);
         // fprintf(stderr, "genomes.palette_index : %d\n", f.genomes->palette_index);
         // fprintf(stderr, "genomes.width : %d\n", f.genomes->width);
         // fprintf(stderr, "genomes.sample_density : %f\n", f.genomes->sample_density);
         // fprintf(stderr, "genomes.contrast : %f\n", f.genomes->contrast);
         // fprintf(stderr, "genomes.gamma : %f\n", f.genomes->gamma);
         
         // //call that actually goes and renders the strip (which is the whole picture when nstrips = 1)
         if (flam3_render(&f, strip_start, flam3_field_both, channels, transparency, &stats))
         {
            fprintf(stderr, "error rendering image: aborting.\n");
            exit(1);
         }

         // want to inspect the output a bit immediately, looks like data at this point is 0 for web but not for native, progress!
         // for (int xi = 0; xi < 80; xi += 8)
         // {
         //    fprintf(stderr, "data at mem index %ld: %ld\n", ((long)strip_start) + xi, *(((long *)strip_start) + xi));
         // }
         // fprintf(stderr, "\n end debug info\n\n");

         if (NULL != out)
         {
            strcpy(fname, out);
         }
         else if (name_enable && cps[i].flame_name[0] > 0)
         {
            sprintf(fname, "%s.%s", cps[i].flame_name, format);
         }
         else
         {
            sprintf(fname, "%s%05d.%s", prefix, i, format);
         }
         if (verbose)
         {
            fprintf(stderr, "writing %s...", fname);
         }
         fp = fopen(fname, "wb");
         if (NULL == fp)
         {
            perror(fname);
            exit(1);
         }

         /* Generate temp file with genome */
         fpc.genome = flam3_print_to_string(f.genomes);

         sprintf(badval_string, "%g", stats.badvals / (double)stats.num_iters);
         fpc.badvals = badval_string;
         sprintf(numiter_string, "%g", (double)stats.num_iters);
         fpc.numiters = numiter_string;
         sprintf(rtime_string, "%d", stats.render_seconds);
         fpc.rtime = rtime_string;

         if (!strcmp(format, "png"))
         {

            write_png(fp, image, cps[i].width, real_height, &fpc, f.bytes_per_channel);
            // fp is the file handle, image is a void *, fpc is flam3 image comments
         }
         else if (!strcmp(format, "jpg"))
         {

            write_jpeg(fp, (unsigned char *)image, cps[i].width, real_height, &fpc);
         }
         else
         {
            fprintf(fp, "P6\n");
            fprintf(fp, "%d %d\n255\n", cps[i].width, real_height);
            if (this_size != fwrite((unsigned char *)image, 1, this_size, fp))
            {
               perror(fname);
            }
         }
         /* Free string */
         free(fpc.genome);

         fclose(fp);
      }

      /* restore the cps values to their original values */
      cps[i].sample_density /= nstrips;
      cps[i].height = real_height;
      cps[i].center[1] = center_y;

      if (verbose)
      {
         fprintf(stderr, "done.\n");
      }
   } //end of for loop

   if (verbose && ((ncps > 1) || (nstrips > 1)))
   {
      long total_time = (long)time(0) - start_time;

      if (total_time > 100)
         fprintf(stderr, "total time = %.1f minutes\n", total_time / 60.0);
      else
         fprintf(stderr, "total time = %ld seconds\n", total_time);
   }

   for (i = 0; i < ncps; i++)
   {

      xmlFreeDoc(cps[i].edits);
      clear_cp(&cps[i], 0);
   }
   free(cps);

   free(image); // maybe don't free image O.o sneaky sneaky
   return 0;
}

EMSCRIPTEN_KEEPALIVE
int get_image_pointer()
{
   return (int)image;
}

EMSCRIPTEN_KEEPALIVE
int get_image_size()
{
   return (int)this_size;
}

EMSCRIPTEN_KEEPALIVE
void set_quality(double quality){
   qs = quality;
}