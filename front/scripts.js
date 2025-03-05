
function requestImage() {
    img_file = document.getElementById("input_file").files[0].name;
    fetch("/image/" + img_file)
    .then(response => response.blob())
    .then(blob => {
        const imgUrl = URL.createObjectURL(blob);
        document.getElementById("imageContainer").src = imgUrl;

    })
}