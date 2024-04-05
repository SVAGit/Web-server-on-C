let dataBase = [
    {image:"https://upload.wikimedia.org/wikipedia/ru/b/b9/Intouchables.jpg",title:"1+1: Intouchables",grade:"10/10"},
    {image:"https://avatars.mds.yandex.net/get-kinopoisk-image/1629390/b7e91316-caf4-4678-9b75-e83b0e65acef/orig",title:"Spider-man 3",grade:"9.7/10"},
    {image:"https://avatars.mds.yandex.net/get-kinopoisk-image/1599028/e1ec4209-569e-4745-a324-b180af8097ef/600x900",title:"Picky blinders",grade:"9/10"},
    {image:"https://avatars.mds.yandex.net/get-kinopoisk-image/1600647/f6b2f7b0-8a46-4c39-88e3-8d6b1bfb2aa6/orig",title:"Avatar",grade:"10/10"},
    {image:"https://avatars.mds.yandex.net/get-kinopoisk-image/1946459/cb7ab888-8441-4aee-b31f-60105cfb32db/orig",title:"Harry Potter",grade:"10/10"},
    {image:"https://ir.ozone.ru/s3/multimedia-z/c1000/6670060379.jpg",title:"Fight club",grade:"7/10"}
]

function getRandomElement(arr) {
    let randIndex = Math.floor(Math.random() * arr.length);
    return arr[randIndex];
}

let button = document.querySelectorAll(".button");
let div = document.querySelectorAll(".div");
let counter = 0;

button[0].addEventListener('click', ()=>{
    let random = [0, 0, 0];
    for(let i = 0; i < 3; i++){
        let flag = 0;
        while(flag == 0){
            flag = 1;
            random[i] = getRandomElement(dataBase);
            for(let j = 0; j < i; j++){
                if(random[i] == random[j]){
                    flag = 0;
                    break;
                }
            }
        }
        smoothly(div[i].querySelector(".img"), "src", random[i].image);
        smoothly(div[i].querySelector(".film-title"), "textContent", random[i].title);
        smoothly(div[i].querySelector(".opisanie"), "textContent", random[i].grade);
    }
});

button[1].addEventListener('click', ()=>{
    window.location.href = "http://localhost:3000/catalog";
});

let style = document.getElementById("style");
let count = 0;
button[2].addEventListener('click', ()=>{
    if(count == 0){
        style.href = "main/white.css";
        button[2].textContent = "Night theme";
        count = 1;
    }else{
        style.href = "main/style.css";
        button[2].textContent = "Day theme";
        count = 0;
    }
});

let random;
    for(let i = 0; i < 3; i++){
        random = getRandomElement(dataBase);
        div[i].querySelector(".img").src = random.image;
        div[i].querySelector(".film-title").textContent = random.title;
        div[i].querySelector(".opisanie").textContent = random.grade;
    }

let loginButton = document.getElementById("loginButton");
loginButton.addEventListener('click', ()=>{
    window.location.href = "http://localhost:8080/login";
});