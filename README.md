# [MI_100_NIGHTMARE](https://github.com/macgyver3/MI_100_NIGHTMARE)

#10=0x10 c'est évident

Faudra bien expliquer le projet là  

![aaaa](https://i.imgflip.com/68nvks.jpg)



Dans le reste faire attention sur le code de retour des fonctions ``1 == Succès`` ET  ``0 == Erreur``

On pourra ajouter un `` enum `` pour être plus précis dans l'erreur à passer en temps que pointeur aux fonctions. *Je l'explique plus loins tkt*
 - [ ] API Serial complète sans ``enum`` erreur
 - [ ] ``enum`` erreur fait
 - [ ] Fonctions interrupteur et LEDs

Serial
---
```c
int RX_available()
```
Renvoi 1 si buffer de réception avec au minimum 1 char sinon 0
```c
int TX_available()
```
Renvoi 1 si le buffer TX à encore de la place sinon 0
```c
void begin(int baudRate)
```
Configure tous les registres relatif au Serial avec le baud rate spécifié
```c
void flush()
```
Fonction bloquante jusqu'à la fin de l'envoi du buffer TX
```c
int print(char toPrint)
```
Met dans le buffer de TX ``toPrint`` si il y a de la place renvoi 1 sinon renvoi 0
>On pourra en faire une surcharge pour tableau de char
```c
char read()
```
Retourne un char si il y en a un dans le buffer de RX
```c
char* read_for(int count)
```
Pour le coup je suis pas sûr de comment on fait ptdr
```c
void interrupt en reception configurable????
```
Va pas te tuer là dessus on va faire un coup de chamanisme ça va bien se passer tkt pas

I/O
---
```c
void begin_inter()
```
Aller hop hop hop tu me configure tout les registres

 - Pin en lecture
 - Désactive Pull-up (si il y a)
 - Un truc que j'ai sûrement oublié

```c
void begin_led()
```
Deuxième édition tu me configure tout les registres

 - Pin en écriture
 - Désactive fonction analogique
 - Un truc que j'ai sûrement oublié
```c
unsigned char read_inter()
```
Retourne le registre des interrupteurs
```c
void write_led(unsigned char buffer)
```
Ecrit dans le registre des LEDs ``buffer``

Fonction Bluetooth
---
```c
void jesaispasencore()
```
Va falloir chercher comment qu'on lui cause au module et quels pins c'est

**ATTENDS MOI JE VEUX JOUER AUSSI**

--------------
![Alt Text](https://c.tenor.com/Dc5SGhuskTUAAAAC/old-guy-falling-down-stairs.gif)  

Quand tu pensais avoir compris ce que j'écrivais et que tu lis la suite

Explication de ma merdasse avec l'enum
---
Par exemple :
```c
void begin(int baudRate)
```
Pourrais avoir un ``int `` de retour pour indiquer une erreur mais aussi un  ``int `` en pointer genre
```c
int begin(int baudRate, int *erreur)
```
* Le ``int`` de retour permet de savoir si ça a fonctionné
* Le ``erreur`` peux être lu si erreur et avoir des valeurs genre ``[Invalid_baudrate,c_cassé,clement_arrete_les_pauses]`` pour être plus précis et agir en conséquence



Crédit
---
- cberthelo007

- picazenave



> Written with [StackEdit](https://stackedit.io/).
> Et plus de temps que prévu
> C'est trop drôle d'écrire en Markdown j'avais oublié ehehe
