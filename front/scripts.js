
function requestImage() {
    img_file = document.getElementById("input_file").files[0].name;
    fetch("/image/" + img_file)
    .then(response => response.blob())
    .then(blob => {
        const imgUrl = URL.createObjectURL(blob);
        document.getElementById("original_img").src = imgUrl;

    })
}

function transform() {
    let img_file = document.getElementById("input_file").files[0].name;
    let transform = document.getElementById("transform_select");
    let spinbox = document.getElementById("spinbox");
    let arg = 0;
    if (spinbox) {
        arg = document.getElementById("spinbox").value;
        arg = parseFloat(arg);
        if (transform.value == "rotate") {
            arg = arg * Math.PI / 180;
        }
    }
    fetch("/" + img_file + "/transform/" + transform.value + "/" + arg)
    .then(response => response.blob())
    .then(blob => {
        const imgUrl = URL.createObjectURL(blob);
        document.getElementById("transformed_img").src = imgUrl;
    })
}

document.getElementById("transform_select").addEventListener("change", function() {
    let container = document.getElementById("args");
    container.innerHTML = "";

    if (this.value == "rotate") {
        let input = document.createElement("input");
        input.type = "number";
        input.id = "spinbox";
        input.min = 0;
        input.max = 359;
        input.value = 1;
        input.style.marginLeft = "10px";
        container.appendChild(input);
    } else if (this.value == "blur") {
        let input = document.createElement("input");
        input.type = "number";
        input.id = "spinbox";
        input.min = 0;
        input.max = 10;
        input.value = 1;
        input.style.marginLeft = "10px";
        container.appendChild(input);
    }
})