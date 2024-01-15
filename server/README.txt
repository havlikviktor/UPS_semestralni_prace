Kompilace:

Kompilaci provedem otevřením terminálu v kořenovém adresáři apliakce,
poté provedem přikazy:

1. cmake	// vytvoří konfigurační soubor MakeFile popisující instuce pro přeložení programu
2. make		// vytvoří spustitělný soubor s názvem server_ups

Spustění:

pro přikazu make by se mám v adresáři měl vytvořit spustitelný soubor server_ups
ten spustíme:

./server_ups -a X -p Y -l aa bb		// s použítím příznaků spustíme server. příznak -a udává adresu na které má sever běžet,
					// -p čislo portu -l je seznam lobby, které budou na serveru k dospozici
					// tento server tedy poběží na adrese X poslouchat bude na portu Y a bude obsahovat dvě lobby s nazvy 'aa' a 'bb'