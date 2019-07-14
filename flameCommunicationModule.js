window.onload = function () {

        let x2js = new X2JS();

        let api;
        Module.noInitialRun = true;
        Module.onRuntimeInitialized = () => {

            //setup the folder needed for the palette file 
            // FS.writeFile("/test.flam3", flameInput)
            FS.mkdir("usr");
            FS.mkdir("usr/local");
            FS.mkdir("usr/local/share");
            FS.mkdir("usr/local/share/flam3");
            // FS.writeFile("/usr/local/share/flam3/flam3-palettes.xml", flamePaletts)

            api = {
                test: Module.cwrap('test', 'number', []),
                getImagePointer: Module.cwrap('get_image_pointer', 'number', []),
                getImageSize: Module.cwrap('get_image_size', 'number', []),
                setQuality: Module.cwrap('set_quality', "" , ['number'])
            };
            // console.log(api.test());
        };

        function renderFractal(textValue, quality){
            let parsed = x2js.xml_str2json(textValue);
            let fractal = parsed.flame;
            console.log("rendering fractal: ", fractal);

            //check if the input fractal has its own color 
            if ("colors" in fractal) {
                //if so, grab it, turn it to xml, and put it as palette 0 in file 
                let parsedColor = fractal.colors;
                let outputPalette = `<palette number="0" name="user-submitted" data=${parsedColor._data}`

                FS.writeFile("/usr/local/share/flam3/flam3-palettes.xml", outputPalette)

            }

            // write input directly to file (note this will ignore the colors but that's okay cause we put it in flam3-palettes.xml)
            FS.writeFile("/test.flam3", textValue);

            // set our inputs 
            api.setQuality(quality);

            //call our main function
            Module.callMain();
        }

        function renderButton() {
            let textValue = document.getElementById("fractalInput").value;

            renderFractal(textValue, 1);

            //read the output file and display
            let pngBuffer = FS.readFile("00000.jpg");
            let blob = new Blob([pngBuffer], { type: "image/jpg" })
            const imageUrl = URL.createObjectURL(blob);

            // option b
            let img = document.createElement("img");
            img.src = imageUrl;
            document.body.appendChild(img);

            // option a
            // const img = document.querySelector('img');
            // document.querySelector('img').src = imageUrl;

            // download button
            var download = document.querySelector("a[ download ]");
            download.setAttribute("href", imageUrl);
            console.log(download);
        }

        document.getElementById("renderButton").onclick = renderButton;
    }