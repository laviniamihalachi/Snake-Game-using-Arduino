# Snake-Game-using-Arduino

Lavinia Mihalachi


Snake game 
1.	La inceputul jocului va aparea numele jocului, nivelul si numerele 3,2,1 pentru a da timp jucatorului sa se pregateasca.
2.	Jocul are 3 niveluri.
   -ease: sarpele se poate deplasa oriunde pe matrice si poate trece prin toti peretii iesind pe partea opusa.
  - medium: o margine a matricii este marcata(linia 0/7 sau coloana 0/7). Adica sarpele nu poate trece prin acel perete.
  -hard:o linie(0/7) si o coloana (0/7) de pe matrice sunt marcate.(sarpele nu poate trece prin acei pereti) 
3.	Pentru a incepe jocul, jucatorul trebuie sa apese pe joystick.
4.	La finalul jocului, se va afisa pe ecran “gameOver” si nivelul la care a murit jucatorul.
5.	Treapta de la un nivel la altul se face prin acumularea unui anumit numar de puncte. Pentru un nou joc, se apasa iar pe joystick.
6.	In rest, sarpele se comporta exact ca in jocul original. Vom controla comenzile date prin folosirea joystick-ului.


Componente folosite:
-arduino Uno
-joystick
-8x8 Led Dot Matrix Display
-2 X breadboard
-jumper wires
-max7219
-2 x resistor
-F-M cable

DESCRIERE:
-	Jocul va fi afisat de matricea(8x8 Led Dot Matrix Display), iar sarpele va fi control din joystick. Matricea este conectata la arduino cu ajutorul MAX7219.
CODE
-pentru pozitionarea fructului am folosit functia random()
-pentru sarpe am folosit un vector
-rezultatul primit de la joystick va oferi variabilei “dir” directia in care se va indrepta sarpele
-daca sarpele is mananca coada sau intra intr-un perete acesta va muri
-cand mananca fructul , scorul creste cu 10p. Cand se trece la un nivel nou, scorul se reseteaza.
   
Voi lasa mai jos un video:
https://www.youtube.com/watch?v=l8ipUSZwxOQ&feature=youtu.be
