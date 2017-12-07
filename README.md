# SnakeArduino
Tudosa Elena-Diana, gr. 251

Numele jocului: Snake
Dispozitive folosite: placa Arduino, breadboardurile, matricea led 8x8, driverul 7219, joystick-ul, ecranul lcd.
Pe matrice se va derula jocul propriu-zis. Se incepe cu un ‘sarpe’ avand lungimea unui punct si cu un punct pe ecran spaunat aleator. Controlul sarpelui se realizeaza cu joystick-ul, iar pozitia fiecarui punct al sarpelui va fi retinuta intr-un vector de tip struct (ce retine si linia si coloana punctului). In momentul in care sarpele ajunge la punctul aleator, dimensiunea lui se va mari cu 1 si se va aprinde un nou punct aleator. 
Dificultate:
Easy, medium, hard - Mobility: easy = sarpele se opreste in momentul in care joystick-ul este in pozitia initiala, medium = sarpele se misca continuu la o viteza medie, hard = sarpele se misca continuu la o viteza ridicata. Acestea vor fi selectate inainte de inceperea jocului, pe placuta lcd.
Bonus:
In timpul jocului, pe harta va aparea aleator cate un punct care va se va distinge cumva de cele ce reprezinta ‘mancarea’ (va clipi, va avea alta intensitate, ramane de vazut). Acesta reprezinta un bonus si poate fi de 5 tipuri:
Puncte bonus
Viata
Cresterea vitezei
Scaderea vitezei
Inversarea axelor (stanga=dreapta, sus=jos si invers)
   -      Pentru ultimele 3 tipuri, durata lor va fi una limitata, de aproximativ 5 secunde.


De asemenea, fiecare runda contine 3 vieti.
Scorul va fi dat de lungimea sarpelui.
Jocul se sfarseste in momentul in care jucatorul pierde cele 3 vieti. 
O viata se pierde daca sarpele se ‘musca’.




Momentan am realizat nivelurile easy-medium-hard, fara scor, bonusuri si placa lcd. Prin urmare, pot veni la prezentarea de miercuri ( chiar cu mai mult). 
