const menuToggle = document.getElementById('mobile-menu');
const headernav = document.querySelector('header nav');

const connectButton = document.getElementById('connect-button');
const sendButton = document.getElementById('send-button');
const messageInput = document.getElementById('message-input');
const outputDiv = document.getElementById('output');

let socket = null;

menuToggle.addEventListener('click', () => {
  headernav.classList.toggle('active');
  menuToggle.classList.toggle('active'); // Ajoute la classe active pour l'animation
});

document.getElementById('login-form').addEventListener('submit', function(event) {
  event.preventDefault(); // Empêche le rechargement de la page
  const username = document.getElementById('username').value;
  const password = document.getElementById('password').value;

  // Simulation d'une connexion réussie (tu peux personnaliser cette condition)
  if (username === "user" && password === "pass") {
      document.getElementById('output').innerText = 'Connexion réussie !'; // Afficher le message de succès
      document.getElementById('message-form').style.display = 'block'; // Afficher le formulaire de message
      document.getElementById('login-form').style.display = 'none'; // Masquer le formulaire de connexion
      
  } else {
      document.getElementById('output').innerText = 'Erreur de connexion : identifiants invalides.'; // Afficher les erreurs
  }
});

document.getElementById('message-form').addEventListener('submit', function(event) {
  event.preventDefault(); // Empêche le rechargement de la page
  const message = document.getElementById('message-input').value;

  // Simuler l'envoi de message et l'afficher
  document.getElementById('output').innerText = 'Message envoyé : ' + message; // Afficher le message de succès
  document.getElementById('message-input').value = ''; // Réinitialiser le champ de message
});


connectButton.addEventListener('click', () => {
  const domainName = '...'; // Remplacez par votre domaine ou votre service DDNS
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
