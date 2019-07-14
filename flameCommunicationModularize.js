import wasmModule from "./flam3-render.js";


window.onload = function () {
    function renderFractalAndShow(textValue, quality, showElement){
        const instance = wasmModule({
            onRuntimeInitialized() {
            //setup the folder needed for the palette file 
            let flamePaletts = `<palette number="0" name="user-submitted" data="00B9896300B6856800B4816E00BA887100AF907100AD937000AC8B6A00AC8365
            00A1816B009680720093816F0091826D007D845A00777A560072715300767558
            007B7A5E00817B6700887C70009C8B7100B6A38300D4C59E00DBCFB000E2DAC3
            00E2D8C500E2D6C800E4D8C800E6DBC500E3DBC400E2D9BF00E2D7BB00DDCDB0
            00D9C4A500D4C5A100D0C79E00D6C99F00D9CEAE00E0D5B900D7CBAC00CFC2A0
            00CFBF9900D0BD9300C1B38600BFA27600B9A17100B6A17200B3A27400B4A475
            00B5A77600BBB57B00C1B68800C9BC9200D2B89700CFBC9C00D0BF9B00D1C29B
            00CFC19A00CEC09900CDB6A400BAB4A400AFA29A00A8948800A2877600A68671
            00AA866C00AB876D00BD916C00BC957400C4A08000D4BFA000D5BFA600D6BFAD
            00D6C8AE00D0C4AE00D4BFA000D3B69400CFB18F00C9AA8B00C3A48700BDA486
            00B7A48600AE9A7F00A1A37400A09665008D895C008184590084855C0088865F
            008F87600098856700A5987500AF9C7200AE956C00A285600096755400926F4F
            008E6A4A00955F45008F5E35008F6334008D6132008E563F0087523B00814F38
            008661340083623500776235007A5A3300000200000001000000000000000000
            00000000000002010071402000844B2E007C4530000000000000000000000000
            00000000002E241B006B5032007E614100886A46008C6E400090733B00937A44
            00A48E5500B79D6C00C3A87900C9B88D00CDBC9100CFBB9A00B7AB9300A48080
            0096807200826E6F0085775D0094794E00A07D5300A07E5400A1805500A8884D
            00A9824B00AB6F5500AC705800A47E5A009D825300927E59008C8E6900959978
            00A29D7500AA8E6900A5835D009C76520095764A008F6C440099654D00A1654D
            00AF725D00B3826400BC967200C3AC8000C9BC9000D2C39C00D9CAA900DED0B5
            00D6C8AE00CEC1AE00C8B49300C2AD8E00C0AD8200C0A47D00C39E8100BEA181
            00B4977700A87F6B0097736300836B53008C724F009D836200B5987600BDA58B
            00CFC39D00D8D3B500E4DABF00DCD4BF00CECABE00D1D0B200C7C49B00C9BE91
            00C3BB9700B8A98000B0A27100A89D65009D824D008C683600724F29001B1612
            0000000000000000000000000000000000000000000000000000010000423429
            006F684C0090785600B29478009D9856009B8844008A61350087693400857134
            00826733007F5E3300784F330071413300000000001813120031262400534839
            00766B4F0078674B007B6347007F5F4D00835B530097663D00906B3A008A7138
            0094773C009E7E41009C8342009B8844009089550096865B009D836200A3815F
            00AA7F5C00A7825C00A5855C00A8875400AB855600A68E5E009B8C5000918A42
            00897D480081714F00876F4B008D6D4700A0825000A78D5900AE996200B39162"`;

            instance.FS.mkdir("usr");
            instance.FS.mkdir("usr/local");
            instance.FS.mkdir("usr/local/share");
            instance.FS.mkdir("usr/local/share/flam3");
            instance.FS.writeFile("/usr/local/share/flam3/flam3-palettes.xml", flamePaletts)

            let api = {
                test: instance.cwrap('test', 'number', []),
                getImagePointer: instance.cwrap('get_image_pointer', 'number', []),
                getImageSize: instance.cwrap('get_image_size', 'number', []),
                setQuality: instance.cwrap('set_quality', "" , ['number'])
            };
            console.log(api.test());

            // let parsed = x2js.xml_str2json(textValue);
            // let fractal = parsed.flame;

            instance.FS.writeFile("/test.flam3", textValue);

            // console.log("rendering fractal: ", fractal);
            console.log('pie');
            api.setQuality(quality);
            instance.callMain();


            //read the output file and display
            let pngBuffer = instance.FS.readFile("00000.jpg");
            let blob = new Blob([pngBuffer], { type: "image/jpg" })
            const imageUrl = URL.createObjectURL(blob);
            showElement.src = imageUrl;

            }
        });
    }

    function renderButton() {
        let textValue = document.getElementById("fractalInput").value;

        for(let i = .1; i < 5; i++){
            let img = document.createElement("img");
            document.body.appendChild(img);
            renderFractalAndShow(textValue, i, img);
        }

        // // download button
        // var download = document.querySelector("a[ download ]");
        // download.setAttribute("href", imageUrl);
        // console.log(download);
    }

    document.getElementById("renderButton").onclick = renderButton;
}