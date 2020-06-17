# **Instrukcja i wyjaśnienia**

Pracownia 2 z Sieci Komputerowych

1. **Główne struktury i zmienne w programie**

Wszystkie wymienione tutaj zmienne są globalne dla programu. Dzięki temu nie potrzeba ich przesyłać do funkcji jako argumenty, a więc kod staje się bardziej czytelny. Ich definicje są zamieszczone w pliku **utils.h**. Domyślnie przyjąłem, że w schemacie jest nie więcej niż 5 komputerów. Aby program działał poprawnie dla większych schematów, należy zwiększyć zmienną **IPS** w pliku **utils.h**.

   * **char table[IPS][tELEMS][STRL]**

Jest to jedna z głównych tablic routingu, przechowująca ciągi znakowe długości **STRL**. Każdy wiersz przechowuje oddzielny wpis routingu. Lewa pozycja to adres danej podsieci, zaś prawa to informacja przez jaki adres można się dostać do tej sieci. Na samym początku programu (przed odebraniem pakietów od sąsiadów) są to po prostu informacje o bezpośrednio połączonych sieciach. Jeśli sieć jest podłączona bezpośrednio, jej prawy wpis zawiera po prostu „0&quot;.

|   |   |
| --- | --- |
| Subnet IP 0 | Via IP 0 |
| Subnet IP 1 | Via IP 1 |
| … | … |
| Subnet IP ( **n** -2) | Via IP ( **n** -2) |
| Subnet IP ( **n** -1) | Via IP ( **n** -1) |

   * **int intTable[IPS][iELEMS]**

Druga główna tablica, przechowująca wartości liczbowe. Każdy wpis odpowiada wpisowi na tej samej pozycji w tablicy **table**. Jej lewa strona przechowuje maski danych podsieci, zaś prawa – odległości do nich. Na samym początku programu są tutaj po prostu informacje o sąsiadach.

|   |   |
| --- | --- |
| Subnet mask 0 | Distance 0 |
| Subnet mask 1 | Distance 1 |
| … | … |
| Subnet mask ( **n** -2) | Distance ( **n** -2) |
| Subnet mask ( **n** -1) | Distance ( **n** -1) |

   * **int turns[IPS]**

Tablica tur, czyli licznik przechowujący informację czy w danej turze otrzymaliśmy informację o dostępności danej podsieci. Tutaj także każdy wpis odpowiada wpisom w dwóch poprzednich tablicach.

   * **char myAddresses[IPS][2][STRL]**

Tablica, która przechowuje informacje o własnych adresach danej instancji programu router. Jeśli do programu zostaną przykładowo podane adresy: 172.16.1.3 oraz 192.168.1.5, to znajdą się one w tej tablicy po lewej stronie, jako ciąg znakowy. Prawa strona tablicy przechowuje domyślną odległość do tej sieci, podaną na wejściu przez użytkownika. Należy ją zapisać, aby przywrócić początkowy stan tablicy routingu w razie zerwania połączenia.

|   |   |
| --- | --- |
| My address 0 | My distance 0 |
| My address 1 | My distance 1 |
| … | … |
| My address ( **k** -2) | My distance ( **k** -2) |
| My address ( **k** -1) | My distance ( **k** -1) |

   * **int n**

Zmienna, która przechowuje aktualną ilość wpisów w tablicy routingu. Początkowo jest równa ilości własnych adresów programu, zwiększa się wraz z dodaniem kolejnego wpisu do tablicy.

   * **int k**

Przechowuje ilość adresów własnych programu. Początkowo jest równa **n**. Program musi wiedzieć które pierwsze **k** wpisów to jego podsieci, aby poprawnie wysyłać tablice routingu.

2. **Stałe zdefiniowane w programie (w celu łatwiejszej zmiany parametrów programu):**
   
* **IPS**

Maksymalna ilość wpisów w tablicy routingu. Domyślnie ustawiona na **5** (czyli schemat może się składać maksymalnie z **5** komputerów).

   * **tELEMS**

Ilość elementów tablicy **table** wszerz. Domyślnie ustawione na **2** , nie powinna być zmieniana.

   * **iELEMS**

Ilość elementów tablicy **intTable** wszerz. Domyślnie ustawione na **2** , nie powinna być zmieniana.

   * **STRL**

Ilość znaków, które mogą przyjmować ciągi znakowe w programie. Domyślnie ustawione na **50**.

   * **MY\_PORT**

Port, którego ma używać program do nasłuchiwania. Domyślnie to **54321** , zgodnie z poleceniem.

   * **UNREACH**

Wartość, którą program ma uznać za odległość „nieskończoną&quot; do danej sieci. Domyślnie jest to
**2<sup>32</sup> -1**, czyli **0xFFFFFFFF**.

   * **TURNUNREACH**

Po tylu turach bez informacji o danej sieci program ma ją uznać za nieosiągalną (unreachable). Domyślnie jest to **5**.

   * **TURNDELETE**

Po tylu turach bez informacji o danej sieci program ma ją uznać za kandydata do usunięcia z tablicy routingu (jeśli nie jest to sieć bezpośrednio połączona). Domyślnie jest to **10**.

   * **UNREACHLIMIT**

Taka odległość jest przez program uznawana za wartość „graniczną&quot;, równą nieskończoności. Jeśli dana sieć jest odległa o **UNREACHLIMIT** lub więcej, traktuje tę sieć jako nieosiągalną. Zapobiega to zjawisku zliczania do nieskończoności. Domyślnie jest to **16**.