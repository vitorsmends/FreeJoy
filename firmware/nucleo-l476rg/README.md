# Joystick USB simulado — NUCLEO-L476RG

Firmware independente para **NUCLEO-L476RG / STM32L476RGT6**. Aparece como
**Nucleo Mock Joystick**, com oito eixos de 16 bits e 16 botões. Envia um relatório
HID de 18 bytes a cada 10 ms. Reutiliza o gerador de mocks do FreeJoy; não implementa
o protocolo do FreeJoy Configurator.

## Hardware: são duas conexões USB

O conector USB da Nucleo está ligado ao **ST-Link**, não ao USB do STM32L476.
Ele programa e alimenta a placa, mas sozinho não fará aparecer um joystick.
Use também um **breakout USB 2.0 com D+, D− e GND**, conectado assim:

| Breakout USB | Nucleo, conector ST morpho |
| --- | --- |
| D+ | PA12 — CN10, pino 12 |
| D− | PA11 — CN10, pino 14 |
| GND | GND — CN10, pino 20 |
| VBUS / 5 V | Deixe desconectado e isolado |

Mantenha a alimentação pelo ST-Link e os jumpers de alimentação/programação na
posição padrão da placa. Não una as saídas de 5 V dos dois cabos USB. Faça as
ligações com a placa desligada e use fios curtos para os sinais USB. Se escolher
um breakout USB-C, ele precisa ter os resistores CC para funcionar como dispositivo.

Este é um arranjo de bancada, com detecção de VBUS desativada. Não é um projeto
elétrico USB para produção. O clock USB usa MSI a 48 MHz calibrado pelo cristal
LSE de 32,768 kHz da Nucleo; placas modificadas ou sem LSE não são suportadas.

Referência de pinagem: [ST UM1724, tabela ST morpho da NUCLEO-L476RG](https://www.st.com/resource/en/user_manual/dm00105823.pdf).

## Compilar no macOS

Instale as ferramentas:

```sh
brew install --cask gcc-arm-embedded
brew install open-ocd
arm-none-eabi-gcc --version
```

Na raiz deste repositório:

```sh
make -C firmware/nucleo-l476rg deps
make -C firmware/nucleo-l476rg
```

`deps` baixa TinyUSB **0.18.0**, no commit fixado em `prepare.py`, e os drivers
CMSIS/HAL fixados pelo TinyUSB. Requer Git, Python 3 e internet. Os downloads ficam
em `.deps/`; nosso código é copiado para um exemplo/board próprios nesse checkout.

Arquivos gerados:

- `firmware/nucleo-l476rg/build/joystick.elf`
- `firmware/nucleo-l476rg/build/joystick.hex`
- `firmware/nucleo-l476rg/build/joystick.bin`

## Gravar

Conecte a Nucleo pelo USB do **ST-Link** e execute:

```sh
make -C firmware/nucleo-l476rg flash
```

O comando usa OpenOCD para programar, verificar e reiniciar. Substitui o firmware
que estava na Nucleo. Este firmware começa em **0x08000000**; não use o endereço
0x08002000 da aplicação FreeJoy/Blue Pill e não grave o bootloader FreeJoy.

Alternativamente, use STM32CubeProgrammer via ST-Link com `joystick.hex`, ou
`joystick.bin` no endereço `0x08000000`.

## Conferir no Mac

Com a Nucleo alimentada pelo ST-Link, conecte também o USB do breakout ao Mac.
Procure **Nucleo Mock Joystick** em Informações do Sistema → USB ou execute:

```sh
system_profiler SPUSBDataType
```

No aplicativo que lê joysticks/HID, espere:

- Oito eixos (X, Y, Z, Rx, Ry, Rz, Slider, Dial), de -32767 a 32767,
  em ondas triangulares de oito segundos, defasadas em 500 ms.
- Botões 1 a 16, cada um pressionado por 500 ms e solto por 500 ms.
- LED LD2 piscando rapidamente sem enumeração; um pulso por segundo quando
  o USB está montado. O LED não substitui a verificação dos relatórios no host.

O dispositivo usa VID/PID de exemplo TinyUSB `CAFE:4004`, apenas para teste local.
Se seu aplicativo filtra pelo VID/PID ou pelo protocolo do FreeJoy, ajuste-o
para este joystick HID genérico. Não há report ID: os dois primeiros bytes são
os 16 botões; seguem oito inteiros de 16 bits com sinal, little-endian.

Se aparecer apenas ST-Link, confira a segunda conexão USB, D+/D−, terra comum e
cabo de dados. A enumeração e a recepção dos relatórios precisam ser validadas na
placa real; compilar e passar nos testes locais não comprova a ligação elétrica.

## Teste local do formato do relatório

Na raiz do repositório:

```sh
cc -std=c99 -Wall -Wextra -Werror -Iapplication/Inc -Ifirmware/nucleo-l476rg/src tests/nucleo_report_test.c -o /tmp/nucleo-report-test
/tmp/nucleo-report-test
```

O suporte de placa deriva do BSP STM32L476 Discovery do TinyUSB (licença MIT
preservada em `board/board.h`). TinyUSB e drivers ST permanecem nas dependências,
com suas respectivas licenças; o linker script local usa somente SRAM1.

Depois de `make deps`, execute `make -C firmware/nucleo-l476rg test` para testar
também os descritores USB/HID reais: tamanho do relatório, endpoint, intervalo,
coleção joystick e strings.
