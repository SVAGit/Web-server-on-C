if(typeof document != "undefined"){
    let main = document.querySelector(".main");
    let username = document.getElementById("username");
    let password = document.getElementById("password");
    let signup = document.getElementById("signup");
    let signin = document.getElementById("signin");
    let lookpassword = document.getElementById("lookpassword");

    main.addEventListener("click", ()=>{
        window.location.href = "http://localhost:8080";
    });

    signup.addEventListener('click', ()=>{
        window.location.href = "http://localhost:8080/login";
    });


    signin.addEventListener('click', (event)=>{
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
        alert(`${username.value},вы успешно зарегистрировались`);
    });

    lookpassword.addEventListener('mousedown', ()=>{
        password.type = "text";
    });
    lookpassword.addEventListener('mouseup', ()=>{
        password.type = "password";
    });
}