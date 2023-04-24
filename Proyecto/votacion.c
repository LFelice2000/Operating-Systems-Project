/* Intento de proclamación de ganador */
        if(sem_trywait(&sistema->winner) == 0){

            /* Soy el ganador */

            got_SIGUSR2 = 0;

            /* Actualización del bloque */
            sem_wait(&sistema->mutex);

            sistema->current.solution = target;
            sistema->winner = getpid();

            /* Inicialización de los votos a -1 */
            for(i = 0; i < MAX_MINEROS; i++){
                sistema->votes[i] = -1;
            }

            sem_post(&sistema->mutex);

            /* Envío señal SIGUSR2 a los demás mineros */
            sem_wait(&sistema->mutex);
            for(i = 0; i < MAX_MINEROS; i++){
                if(sistema->pids[i] != -1 && sistema->pids[i] != getpid()){
                    kill(sistema->pids[i], SIGUSR2);
                }
            }
            sem_post(&sistema->mutex);

            /* Comprobar que todos los demás procesos han votado */
            j = 0;
            while(j < MAX_MINEROS){
                sem_wait(&sistema->mutex);
                for(i = 0; i < MAX_MINEROS; i++){
                    if(sistema->votes[i] != -1){
                        count++;
                    }
                }

                if(count == sistema->n_mineros - 1){
                    sem_post(&sistema->mutex);
                    count = 0;
                    break;
                }

                count = 0;

                sem_post(&sistema->mutex);
                if(i == MAX_MINEROS){
                    break;
                }

                usleep(500);
                j++;
            }

            /* Recuento de votos */
            sem_wait(&sistema->mutex);
            for(i = 0; i < MAX_MINEROS; i++){
                if(sistema->votes[i] == 1){
                    yes++;
                }else if(sistema->votes[i] == 0){
                    no++;
                }
            }

            /* Si se recibe la aprobación, actualizar Bloque */
            if(yes > no){
                /* Número total de votos */
                sistema->current.votes = yes + no;

                /* Número de votos positivos */
                sistema->current.positives = yes;

                /* El minero ganador, gana una moneda y se guarda en la cartera */
                for(i = 0; i < MAX_MINEROS; i++){
                    if(sistema->current.wallets[i].pid == getpid()){
                        sistema->current.wallets[i].coins += 1;
                    }
                }

            }

            sem_post(&sistema->mutex);

            /* Enviar bloque resulto a Monitor mediante cola de mensajes */
            /* Sincronizar con semáforos con nombre */

            /* Preparar bloque para nueva ronda */
            sem_wait(&sistema->mutex);
            bloque_init(&sistema->current, sistema, target);
            sem_post(&sistema->mutex);

            /* Liberar el semáforo winner */
            sem_post(&sistema->winner);

            /* Enviar señal SIGUSR1 a los demás mineros */
            sem_wait(&sistema->mutex);
            for(i = 0; i < MAX_MINEROS; i++){
                if(sistema->pids[i] != -1 && sistema->pids[i] != getpid()){
                    kill(sistema->pids[i], SIGUSR1);
                }
            }
            sem_post(&sistema->mutex);

            got_SIGUSR1 = 1;

            /* Enviar Bloque al proceso Registrador */
            if(write(fd[1], &sistema->current, sizeof(Bloque)) == -1){
                perror("[ERROR] No se ha podido enviar el bloque al proceso Registrador\n");
                exit(EXIT_FAILURE);
            }

        }else{

            /* No soy el ganador */

            /* Esperar a recibir la señal SIGUSR2 */
            while(got_SIGUSR2 == 0){
                sigsuspend(&oldset);
            }

            got_SIGUSR2 = 0;

            /* Votar */
            sem_wait(&sistema->mutex);

            if(sistema->current.solution == target){

                /* Votar a favor */
                for(i = 0; i < MAX_MINEROS; i++){
                    if(sistema->votes[i] == -1){
                        sistema->votes[i] = 1;
                        break;
                    }
                }

            }else{

                /* Votar en contra */
                for(i = 0; i < MAX_MINEROS; i++){
                    if(sistema->votes[i] == -1){
                        sistema->votes[i] = 0;
                        break;
                    }
                }

            }

            sem_post(&sistema->mutex);

            while(got_SIGUSR1 == 0){
                sigsuspend(&oldset);
            }

            /* Enviar Bloque al proceso Registrador */
            if(write(fd[1], &sistema->current, sizeof(Bloque)) == -1){
                perror("[ERROR] No se ha podido enviar el bloque al proceso Registrador\n");
                exit(EXIT_FAILURE);
            }

        }