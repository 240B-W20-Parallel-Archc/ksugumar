make

#sleep(3)

  #Read - Threads - Random
  for t in 1 2 4 8 12 16 
  do
  	echo -e "Doing Read Random with $t threads...\n"
  	./latency -t $t
  done
  
  
  
 #Read-Write - Threads - Random
 for t in 1 2 4 8 12 16 
 do
 	echo -e "Doing Read-Write Random with $t threads..\n"
 	./latency -t $t -rw
 done
 
 
 
 #Read - Threads - Strided
 for t in 1 2 4 8 12 16 
 do
 	echo -e "Doing Read Strided with $t threads..\n"
 	./latency -t $t -s
 done
 
 
 
 #Read-Write - Threads - Strided
 for t in 1 2 4 8 12 16 
 do
 	echo -e "Doing Read-Write Strided with $t threads..\n"
 	./latency -t $t -s -rw
 done

