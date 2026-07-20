# Transmisja Interfejsu

## Sposób uruchomienia

![Ikona USART](Sources/Images/usartikona.webp)<br>
[źródło][1]

<div align="justify">
Aby poprawnie przetestować działanie tego programu (tzw. test sprzętowej pętli zwrotnej / hardware loopback), użyłem tricku z wysyłaniem danych z portu do samego siebie. Polega to na zwarciu dwóch pinów: <strong>TX (Transmit / Nadawanie)</strong> oraz <strong>RX (Receive / Odbiór)</strong> za pomocą kawałka folii aluminiowej (lub przewodu/zworki). Dzięki temu to, co program wysyła przez nadajnik, jest natychmiastowo odbierane przez odbiornik.
</div>

### Kompilacja i uruchomienie

Aby skompilować program za pomocą kompilatora g++, wpisz w terminalu:

```bash
g++ SendText.cpp
```

Po udanej kompilacji uruchom program wpisując:

```bash
./a.exe
```

## Technologie

Program napisany w C++ dla systemu Windows. Wykorzystuje następujące technologie i biblioteki:

* **Kompilator:** g++ (MinGW-W64 13.2.0)
* **API Systemu:** Win32 API (`windows.h`) – do konfiguracji i komunikacji z portem szeregowym (użycie m.in. `CreateFile`, `WriteFile`, `ReadFile`, struktur `DCB` i `COMMTIMEOUTS`).
* **Biblioteka Standardowa C++:** strumienie wejścia/wyjścia (`iostream`), obsługa plików (`fstream`) oraz typ łańcuchowy (`string`).

## Przykładowy test (Payload)

W programie zaimplementowano proste menu umożliwiające wczytanie predefiniowanego payloadu testowego. Po zwarciu pinów TX i RX, uruchomieniu aplikacji i wybraniu payloadu `Dinosaur` o wielkości bloku `1` bajt, program:
1. Odczytuje gotowy plik z rysunkiem ASCII-Art.
2. Wysyła znak po znaku (blok po 1 bajcie) na port szeregowy i od razu nasłuchuje odpowiedzi na tym samym porcie.
3. Odbiera odesłany ze zworki/folii znak i na bieżąco wypisuje go na ekran. 

![Działanie programu - animacja](Sources/Movies/example.gif)


## Złożoność obliczeniowa

Złożoność czasowa O programu to zawsze **O(n)**, gdzie *n* stanowi całkowitą liczbę znaków (bajtów) w wybranym pliku tekstowym.

**Uzasadnienie:**
<div align="justify">
Wszystkie operacje w kodzie są ściśle i liniowo zależne od wielkości pliku. Pętla wejściowa `while` wczytuje po jednym znaku, więc iteruje dokładnie *n* razy. Następnie, mimo dzielenia pliku na mniejsze porcje (podwójna pętla przy wysyłaniu), liczba cykli redukuje się zawsze do objętości pliku. Przykładowo: przy 100 znakach i bloku rozmiaru 5, pętla zewnętrzna obróci się 20 razy, zaś wewnętrzna przekaże i skopiuje każdorazowo 5 znaków ($20 \times 5 = 100$ operacji). Brak w kodzie procedur potęgujących pracę (np. $O(n^2)$), przez co czas analizy rośnie idealnie proporcjonalnie do objętości wczytanych danych. Prawdziwym "hamulcem" nie jest więc obciążenie procesora, lecz stała szybkość przesyłowa (BaudRate) samego portu COM.
</div>

[1]: https://forbot.pl/blog/port-szeregowy-interfejs-usart-czyli-komunikacja-mikrokontrolera-z-komputerem-id1122
