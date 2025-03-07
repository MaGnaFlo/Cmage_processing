
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
    img_file = document.getElementById("input_file").files[0].name;
    transform_type = document.getElementById("transform_select").value;
    fetch("/" + img_file + "/transform/" + transform_type)
    .then(response => response.blob())
    .then(blob => {
        const imgUrl = URL.createObjectURL(blob);
        document.getElementById("transformed_img").src = imgUrl;
    })
}