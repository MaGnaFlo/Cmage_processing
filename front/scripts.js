
function requestImage() {
    img_file = document.getElementById("input_file").files[0].name;
    fetch("/image/" + img_file)
    .then(response => response.blob())
    .then(blob => {

        // convert blob to png
        const imgUrl = URL.createObjectURL(blob);
        console.log("got", blob);

        document.getElementById("imageContainer").src = imgUrl;

    })
}