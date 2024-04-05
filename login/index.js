if(typeof document != "undefined"){
    let main = document.querySelector(".main");
    let username = document.getElementById("username");
    let password = document.getElementById("password");
    let repeat = document.getElementById("repeat");
    let signup = document.getElementById("signup");
    let signin = document.getElemeentById("signin");
    let lookpassword = document.getElementById("lookpassword");
    let lookrepeat = document.getElementById("lookrepeat");

    main.addEventListener("click", ()=>{
        window.location.href = "http://localhost:8080";
    });

    signin.addEventListener('click', ()=>{
        window.location.href = "http://localhost:8080/signin";
    });
    

    signup.addEventListener('click', (event)=>{
        if(username.value == ""){
            alert("Введите имя пользователя");
            event.preventDefault();
            return 1;
        }
        if(password.value == ""){
            alert("Введите пароль");
            event.preventDefault();
            return 1;
        }
        if(password.value != repeat.value){
            password.value = "";
            repeat.value = "";
            alert("Пароли не совпадают");
            event.preventDefault();
            return 1;
        }
        alert(`${username.value},вы успешно зарегистрировались`);
    });

    lookpassword.addEventListener('mousedown', ()=>{
        password.type = "text";
    });
    lookpassword.addEventListener('mouseup', ()=>{
        password.type = "password";
    });

    lookrepeat.addEventListener('mousedown', ()=>{
        repeat.type = "text";
    });

    lookrepeat.addEventListener('mouseup', ()=>{
        repeat.type = "password";
    });
}