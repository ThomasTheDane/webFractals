import wasmModule from "./flam3-render.js";
import { xml_str2json } from "./xml2json.js"

window.onload = function () {
    let renderStartTime = 0;
    function renderFractalAndShow(textValue, quality, showElement) {
        const instance = wasmModule({
            onRuntimeInitialized() {


                let api = {
                    test: instance.cwrap('test', 'number', []),
                    getImagePointer: instance.cwrap('get_image_pointer', 'number', []),
                    getImageSize: instance.cwrap('get_image_size', 'number', []),
                    setQuality: instance.cwrap('set_quality', "", ['number'])
                };
                console.log("Test of module(41): " + api.test());


                instance.FS.mkdir("usr");
                instance.FS.mkdir("usr/local");
                instance.FS.mkdir("usr/local/share");
                instance.FS.mkdir("usr/local/share/flam3");


                //extract the pallett and put it in the file 
                let parsed = xml_str2json(textValue);
                let fractal = parsed.flame;
                console.log("rendering fractal: ", fractal);

                //check if the input fractal has its own color 
                if ("colors" in fractal) {
                    //if so, grab it, turn it to xml, and put it as palette 0 in file 
                    let parsedColor = fractal.colors;
                    let outputPalette = `<palette number="0" name="user-submitted" data=${parsedColor._data}`

                    instance.FS.writeFile("/usr/local/share/flam3/flam3-palettes.xml", outputPalette)

                }


                instance.FS.writeFile("/test.flam3", textValue);

                // console.log("rendering fractal: ", fractal);
                api.setQuality(quality);
                instance.callMain();

                //read the output file and display
                let pngBuffer = instance.FS.readFile("00000.jpg");
                let blob = new Blob([pngBuffer], { type: "image/jpg" })
                const imageUrl = URL.createObjectURL(blob);
                showElement.src = imageUrl;

                document.getElementById("renderSpinner").hidden = true;

                console.log(`Fractal took [${(new Date().getTime() - renderStartTime)/1000.0}] seconds to render at quality ${quality}`)
            }
        });
    }

    function renderButton() {

        let textValue = document.getElementById("fractalInput").value;

        // Render test 
        // for(let i = .1; i < 5; i++){
        //     let img = document.createElement("img");
        //     document.body.appendChild(img);
        //     renderFractalAndShow(textValue, i, img);
        // }
        let img = document.getElementById("fractalOutput");
        img.src = "";
        document.getElementById("renderSpinner").hidden = false;
        renderStartTime = new Date().getTime();
        renderFractalAndShow(textValue, parseFloat(document.getElementById("qualitySlider").value), img);

        // // download button
        // var download = document.querySelector("a[ download ]");
        // download.setAttribute("href", imageUrl);
        // console.log(download);
    }

    document.getElementById("renderButton").onclick = renderButton;
}