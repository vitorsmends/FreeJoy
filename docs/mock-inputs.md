# Teste USB sem circuito de entradas

Este modo precisa de uma placa STM32 compatível com o FreeJoy e conexão USB.
O próprio firmware gera os dados do joystick, sem potenciômetros ou botões.

Com GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`) instalado, execute na raiz:

```sh
make -C armgcc -f makefile.app MOCK_INPUTS=1
```

Os arquivos ficam em `armgcc/build/app-mock/FreeJoy.{hex,bin,elf}`.
Grave a aplicação pelo procedimento que você já utiliza no FreeJoy. O binário
da aplicação tem endereço de início `0x08002000` e depende do bootloader do
projeto; não é uma imagem completa para gravar no início da flash.

No Keil, adicione `FREEJOY_MOCK_INPUTS=1` aos defines do target **Application**
e faça Rebuild. Não é necessário adicionar arquivos C ao projeto.

Ao conectar a placa, o computador recebe:

- Oito eixos, de -32767 a 32767, em ondas triangulares de oito segundos.
  Cada eixo tem uma defasagem de 500 ms.
- Dezesseis botões: um pressionado por 500 ms, seguido de 500 ms sem nenhum
  pressionado; a sequência completa se repete a cada 16 segundos.
- Os mesmos valores simulados nos relatórios de diagnóstico do configurador.

Confira os eixos e as transições dos botões no aplicativo que você está
integrando. Não é necessário configurar pinos. O perfil é carregado em RAM
a cada inicialização, mantendo a configuração salva na flash. Evite salvar
configurações pelo configurador durante o teste: esse comando continua
gravando na flash normalmente.

A simulação substitui os dados na montagem dos relatórios USB. Não exercita
ADC, sensores, debounce, calibração, curvas, filtros, mapeamento ou saída UART.
Alterações no configurador não controlam as ondas e a sequência simuladas.

Para voltar às entradas reais, compile sem `MOCK_INPUTS=1` e grave a aplicação
normal (`armgcc/build/app/`). No Keil, remova o define e faça Rebuild.

Teste do gerador no computador, sem placa ou compilador ARM:

```sh
cc -std=c99 -Wall -Wextra -Werror -Iapplication/Inc tests/mock_inputs_test.c -o /tmp/freejoy-mock-test
/tmp/freejoy-mock-test
```
