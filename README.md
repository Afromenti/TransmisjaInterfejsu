# Transmisja Interfejsu

## Sposób uruchomienia

![Ikona USART](Sources/Images/usartikona.webp)

<!-- Źródło: https://forbot.pl/blog/port-szeregowy-interfejs-usart-czyli-komunikacja-mikrokontrolera-z-komputerem-id1122 -->

Aby poprawnie przetestować działanie tego programu (tzw. test sprzętowej pętli zwrotnej / hardware loopback), użyłem tricku z wysyłaniem danych z portu do samego siebie. Polega to na zwarciu dwóch pinów: **TX (Transmit / Nadawanie)** oraz **RX (Receive / Odbiór)** za pomocą kawałka folii aluminiowej (lub przewodu/zworki). Dzięki temu to, co program wysyła przez nadajnik, jest natychmiastowo odbierane przez odbiornik.

### Kompilacja i uruchomienie

Aby skompilować program za pomocą kompilatora g++, wpisz w terminalu:

```bash
g++ SendText.cpp
```

Po udanej kompilacji uruchom program wpisując:

```bash
./a.exe
```
