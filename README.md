# codexion
Concurrent programming in C through an intense simulation where coders race against burnout while competing for scarce USB dongles. Implementing POSIX threads, mutexes, condition variables, and scheduling algorithms (FIFO/EDF) to orchestrate resource sharing, prevent deadlocks, and ensure fair access.

## Fase 1 — Pedir para compilar
O coder chega a este ponto (seja no arranque, seja depois de acabar de refatorar) e precisa dos dois dongles (esquerdo e direito) ao mesmo tempo. Isto é a parte complexa que já desenhámos: entra na secção crítica (arbitration_lock), regista o seu arrival_time_ms, e fica em ciclo:

verifica se pode agarrar os dois dongles agora (livres, fora de cooldown, e tem prioridade sobre os vizinhos que também os queiram);
se sim, agarra-os e sai do ciclo;
se não, dorme (timedwait) e volta a verificar quando acorda.

## Fase 2 — Compilar
Assim que tem os dois dongles: regista last_compile_start_ms, imprime os dois logs "has taken a dongle" (fora do lock de arbitragem, já não precisa dele), imprime "is compiling", e dorme (usleep) durante time_to_compile milissegundos — é aqui que "passa o tempo" com os dongles.
## Fase 3 — Libertar
Acabou de compilar: volta a entrar na secção crítica, larga os dois dongles (marca in_use = 0, available_at_ms = agora + cooldown), faz broadcast para acordar quem estava à espera, incrementa compiles_done.
## Fase 4 — Debug
Imprime "is debugging", dorme time_to_debug ms. Não precisa de dongles nesta fase.
## Fase 5 — Refactor
Imprime "is refactoring", dorme time_to_refactor ms. No fim desta fase, volta à Fase 1.
E repete, até stop ser 1.