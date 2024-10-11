const menuToggle = document.getElementById('mobile-menu');
const headernav = document.querySelector('header nav');

const connectButton = document.getElementById('connect-button');
const sendButton = document.getElementById('send-button');
const messageInput = document.getElementById('message-input');
const outputDiv = document.getElementById('output');

var sliderServo = document.getElementById("myRange");
var sliderInfo = document.getElementById("sliderinfo");

let socket = null;

menuToggle.addEventListener('click', () => {
  headernav.classList.toggle('active');
  menuToggle.classList.toggle('active');
});

document.getElementById('login-form').addEventListener('submit', function(event) {
  event.preventDefault();
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;

  if (username === "user" && password === "pass") {
      document.getElementById('output').innerText = 'Connexion réussie !';
      document.getElementById('message-form').style.display = 'block';
      document.getElementById('login-form').style.display = 'none';
      
  } else {
      document.getElementById('output').innerText = 'Erreur de connexion : identifiants invalides.';
  }
});

document.getElementById('message-form').addEventListener('submit', function(event) {
  event.preventDefault();
  const message = document.getElementById('message-input').value;

  document.getElementById('output').innerText = 'Message envoyé : ' + message;
  document.getElementById('message-input').value = '';
});


connectButton.addEventListener('click', () => {
  const domainName = 'XX'; // A remplacer par l'IP serveur
  socket = new WebSocket(`ws://${domainName}`);

  socket.onopen = () => {
    console.log('Connexion établie');
    outputDiv.innerHTML += 'Connexion établie<br>';
  };

  socket.onmessage = (event) => {
    console.log(`Message reçu : ${event.data}`);
    outputDiv.innerHTML += `Message reçu : ${event.data}<br>`;
  };

  socket.onclose = () => {
    console.log('Connexion fermée');
    outputDiv.innerHTML += 'Connexion fermée<br>';
  };

  socket.onerror = (error) => {
    console.log(`Erreur : ${error}`);
    outputDiv.innerHTML += `Erreur : ${error}<br>`;
  };
});

sendButton.addEventListener('click', () => {
  const message = messageInput.value;
  socket.send(message);
  messageInput.value = '';
});

sliderServo.addEventListener('click', () => {
  const message = sliderServo.value;
  sliderInfo.textContent = message;
  socket.send("SERVO:" + message);
  messageInput.value = '';
});