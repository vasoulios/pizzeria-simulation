#include "p3200105-p3200181-pizzeria.h"

// check_mutex
void check_mutex(int  rc) {		//checks if the mutexes where locked and unlocked correctly
	if (rc != 0){	
		printf("Mutex lock-unlock was not performed correctly. Error code: %d\n", rc);
		pthread_exit(&rc);
	}
}

void print_failed(const int oid) {
	printf("Order number %d failed.\n", oid);
}

void print_successful(const int oid) {
	printf("Order number %d was registered.\n", oid);
}

void print_ready(const int oid, int preparation_minutes) {
	printf("Order number %d was ready in %d minutes.\n", oid, preparation_minutes);
}

void print_delivered(const int oid, int delivery_minutes) {
	printf("Order number %d was delivered in %d minutes.\n", oid, delivery_minutes);
}

// mutexes
pthread_mutex_t mutex_available_cook = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_oven = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_packer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_available_deliverer = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_income = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_plain_income = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_special_income = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_failed_orders = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_successful_orders = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexPlain = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSpecial = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_order_preparation_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_delivery_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_order_delivery_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_delivery_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_max_cooling_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_order_cooling_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_cooling_time = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_console = PTHREAD_MUTEX_INITIALIZER;

// condition variables
pthread_cond_t cond_available_cook = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_oven = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_packer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_available_deliverer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_plain = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_special = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_failed_orders = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_successful_orders = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_income = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_plain_income = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_special_income = PTHREAD_COND_INITIALIZER;

// variables
int available_cook; 
int available_oven; 
int available_packer; 
int available_deliverer; 
int income;
int plain_income;
int special_income;
int failed_orders; 
int successful_orders; 
int plain; 
int special; 
unsigned int seed; 

double total_delivery_time;
double order_delivery_time;
double order_preparation_time;
double total_cooling_time;
double order_cooling_time;
double max_delivery_time;
double max_cooling_time;

// order_execution
void * order_execution(void * order_id) {
	
	int rc;
	int oid = *(int *) order_id;
	
	struct timespec delivery_start;
	struct timespec delivery_finish;
	struct timespec waiting_start;
	struct timespec waiting_finish;
	struct timespec cooling_start;
	struct timespec cooling_finish;
	struct timespec prep_start;
	struct timespec prep_finish;
	
	int random_number_of_pizzas;
	int random_delivery_time;
	int random_charge_time;

	clock_gettime(CLOCK_REALTIME, &prep_start);	// time that the packing of the order starts
	clock_gettime(CLOCK_REALTIME, &delivery_start);	// time that the service of the client starts
	
	// random number of pizzas
	random_number_of_pizzas = rand_r(&seed)% (Norderhigh+1);
	if ( random_number_of_pizzas < Norderlow){
		random_number_of_pizzas = Norderlow;
	}

    for (int i=0; i<random_number_of_pizzas; ++i) {
        // plain or special pizza
        bool isPlain = rand_r(&seed) % 1 <= Pplain;
        // if it is a plain pizza
        if(isPlain){
            //increase number of plain pizzas
            rc = pthread_mutex_lock(&mutexPlain);
            check_mutex(rc);
            ++plain;
            pthread_cond_signal(&cond_plain);
            rc = pthread_mutex_unlock(&mutexPlain);
            check_mutex(rc);

            //increase income from plain pizzas
            rc = pthread_mutex_lock(&mutex_plain_income);
            check_mutex(rc);
            plain_income = plain_income + Cplain;
            pthread_cond_signal(&cond_plain_income);
            rc = pthread_mutex_unlock(&mutex_plain_income);
            check_mutex(rc);

            pthread_exit(NULL);
        // if it is a special pizza
        }else{
            //increase number of special pizzas
            rc = pthread_mutex_lock(&mutexSpecial);
            check_mutex(rc);
            ++special;
            pthread_cond_signal(&cond_special);
            rc = pthread_mutex_unlock(&mutexSpecial);
            check_mutex(rc);

            //increase income from special pizzas
            rc = pthread_mutex_lock(&mutex_special_income);
            check_mutex(rc);
            special_income = special_income + Cspecial;
            pthread_cond_signal(&cond_special_income);
            rc = pthread_mutex_unlock(&mutex_special_income);
            check_mutex(rc);
        }
    }
	
	// random time for order's charge time
	random_charge_time = rand_r(&seed)%(Tpaymenthigh +1);
	if ( random_charge_time < Tpaymentlow){
		random_charge_time = Tpaymentlow;
	}
	sleep(random_charge_time);
	
	// success or failure of transaction
	bool successfull_transaction = rand_r(&seed) % 1<= Pfail;
	
	// if the transaction failed
	if(!successfull_transaction){
	    // increase failed orders
		rc = pthread_mutex_lock(&mutex_failed_orders);
		check_mutex(rc);
		++failed_orders;
		pthread_cond_signal(&cond_failed_orders);
		rc = pthread_mutex_unlock(&mutex_failed_orders);
		check_mutex(rc);

		// print that the order has failed
		rc = pthread_mutex_lock(&mutex_console);
		check_mutex(rc);
		print_failed(order_id);
		rc = pthread_mutex_unlock(&mutex_console);
		check_mutex(rc);
		
		pthread_exit(NULL);
	// else if the transaction was succesful
	}else{
	    // increase successful orders
		rc = pthread_mutex_lock(&mutex_successful_orders);
		check_mutex(rc);
		++successful_orders;
		pthread_cond_signal(&cond_successful_orders);
		rc = pthread_mutex_unlock(&mutex_successful_orders);
		check_mutex(rc);

		// increase the income
		rc = pthread_mutex_lock(&mutex_income);
		check_mutex(rc);
		income = income + plain_income + special_income;
		pthread_cond_signal(&cond_income);
		rc = pthread_mutex_unlock(&mutex_income);
		check_mutex(rc);

		// print that the order did not fail
		rc = pthread_mutex_lock(&mutex_console);
		check_mutex(rc);
		print_successful(order_id);
		rc = pthread_mutex_unlock(&mutex_console);
		check_mutex(rc);
	}
	

	rc = pthread_mutex_lock(&mutex_available_cook);
	check_mutex(rc);
	
	// wait until there is an available cook
	while (available_cook <= 0) {
		pthread_cond_wait(&cond_available_cook, &mutex_available_cook);
	}
	--available_cook;
	
	rc = pthread_mutex_unlock(&mutex_available_cook);
	check_mutex(rc);
	
	// wait until all the pizzas are prepared
	sleep(Tprep* random_number_of_pizzas);
	rc = pthread_mutex_lock(&mutex_available_oven);
	check_mutex(rc);
	
	// wait until there are enough available ovens
	while (available_oven <= random_number_of_pizzas) {
		pthread_cond_wait(&cond_available_oven, &mutex_available_oven);
	}
	available_oven = available_oven - random_number_of_pizzas;
	pthread_cond_signal(&cond_available_oven);
	
	// release cook
	rc = pthread_mutex_lock(&mutex_available_cook);
	check_mutex(rc);
	++available_cook;
	pthread_cond_signal(&cond_available_cook);
	rc = pthread_mutex_unlock(&mutex_available_cook);
	check_mutex(rc);

	rc = pthread_mutex_unlock(&mutex_available_oven);
	check_mutex(rc);

	// wait until all the pizzas are baked
	sleep(Tbake);
	
	clock_gettime(CLOCK_REALTIME, &cooling_start);	// time when the cooling of the order starts

	rc = pthread_mutex_lock(&mutex_available_packer);
	check_mutex(rc);
	
	// wait until there is an available packing employee
	while (available_packer <= 0) {
		pthread_cond_wait(&cond_available_packer, &mutex_available_packer);
	}
	--available_packer;
	
	// wait until all the pizzas are packed
	sleep(Tpack * random_number_of_pizzas);
	
	// release ovens
	rc = pthread_mutex_lock(&mutex_available_oven);
	check_mutex(rc);
	available_oven = available_oven + random_number_of_pizzas;
	pthread_cond_signal(&cond_available_oven);
	rc = pthread_mutex_unlock(&mutex_available_oven);
	check_mutex(rc);
	
	// release packing employee
	rc = pthread_mutex_lock(&mutex_available_packer);
	check_mutex(rc);
	++available_packer;
	pthread_cond_signal(&cond_available_packer);
	rc = pthread_mutex_unlock(&mutex_available_packer);
	check_mutex(rc);	
		
	clock_gettime(CLOCK_REALTIME, &prep_finish);	// time when the packing stops
	
	// individual order preparation time
	rc = pthread_mutex_lock(&mutex_order_preparation_time);
	check_mutex(rc);
	order_preparation_time = prep_finish.tv_sec - prep_start.tv_sec;
	rc = pthread_mutex_unlock(&mutex_order_preparation_time);
	check_mutex(rc);
	
	// print that the order is ready
	rc = pthread_mutex_lock(&mutex_console);
	check_mutex(rc);
	print_ready(order_id, order_preparation_time);
	rc = pthread_mutex_unlock(&mutex_console);
	check_mutex(rc);
	
	rc = pthread_mutex_lock(&mutex_available_deliverer);
	check_mutex(rc);
	
	// wait until there is an available delivery employee
	while (available_deliverer <= 0) {
		pthread_cond_wait(&cond_available_deliverer, &mutex_available_deliverer);
	}
	--available_deliverer;

	rc = pthread_mutex_unlock(&mutex_available_deliverer);
	check_mutex(rc);

	random_delivery_time = rand_r(&seed)% (Tdelhigh+1);
	if ( random_delivery_time < Tdellow){
		random_delivery_time = Tdellow;
	}
	// wait for the order to be delivered
	sleep(random_delivery_time);

	clock_gettime(CLOCK_REALTIME, &cooling_finish); 	// time when the cooling of the order stops
	clock_gettime(CLOCK_REALTIME, &delivery_finish);	// time when the client service is completed
	
	// wait for the delivery employee to come back
	sleep(random_delivery_time);

	// release delivery employee
	rc = pthread_mutex_lock(&mutex_available_deliverer);
	check_mutex(rc);
	++available_deliverer;
	pthread_cond_signal(&cond_available_deliverer);
	rc = pthread_mutex_unlock(&mutex_available_deliverer);
	check_mutex(rc);

	// total and individual order delivery time
	rc = pthread_mutex_lock(&mutex_order_delivery_time);
	check_mutex(rc);
	order_delivery_time = delivery_finish.tv_sec - delivery_start.tv_sec;
	rc = pthread_mutex_unlock(&mutex_order_delivery_time);
	check_mutex(rc);
	rc = pthread_mutex_lock(&mutex_total_delivery_time);
	check_mutex(rc);
	total_delivery_time += order_delivery_time;
	rc = pthread_mutex_unlock(&mutex_total_delivery_time);
	check_mutex(rc);
	rc = pthread_mutex_lock(&mutex_max_delivery_time);
	check_mutex(rc);
	if (order_id == 0) {
		max_delivery_time = order_delivery_time;
	}
	if (order_delivery_time > max_delivery_time) {
		max_delivery_time = order_delivery_time;
	}
	rc = pthread_mutex_unlock(&mutex_max_delivery_time);
	check_mutex(rc);
	
	// print that the order was delivered
	rc = pthread_mutex_lock(&mutex_console);
	check_mutex(rc);
	print_delivered(order_id, order_delivery_time);
	rc = pthread_mutex_unlock(&mutex_console);
	check_mutex(rc);

	// total and individual order cooling time
	rc = pthread_mutex_lock(&mutex_order_cooling_time);
	check_mutex(rc);
	order_cooling_time = cooling_finish.tv_sec - cooling_start.tv_sec;
	rc = pthread_mutex_unlock(&mutex_order_cooling_time);
	check_mutex(rc);
	rc = pthread_mutex_lock(&mutex_total_cooling_time);
	check_mutex(rc);
	total_cooling_time += order_cooling_time;
	rc = pthread_mutex_unlock(&mutex_total_cooling_time);
	check_mutex(rc);
	rc = pthread_mutex_lock(&mutex_max_cooling_time);
	check_mutex(rc);
	if (order_id == 0) {
		max_cooling_time = order_cooling_time;
	}
	if (order_cooling_time > max_cooling_time) {
		max_cooling_time = order_cooling_time;
	}
	rc = pthread_mutex_unlock(&mutex_max_cooling_time);
	check_mutex(rc);
	
	pthread_exit(NULL);
}


// main
int main(int argc, char *argv[]) {

    // check if parameters given were correct
	if (argc != 3) {
		printf("The program must accept exactly two parameters: the number of customers to be served, Νcust and the random seed for the random number generator. \n");
		exit(-1);
	}
	int number_of_orders = atoi(argv[1]);
	if (number_of_orders <= 0){
		printf("Number of pizzas must be a positive integer number\n");
		exit(-1);
	}
	
	total_delivery_time = 0;
	order_delivery_time = 0;
	total_cooling_time = 0;
	order_cooling_time = 0;
	order_preparation_time = 0;
	max_delivery_time = -1;
	max_cooling_time = -1;

	seed = atoi(argv[2]);
	
	income = 0;
    plain_income = 0;
    special_income = 0;
	plain = 0;
    special = 0;
	failed_orders = 0;
	successful_orders = 0;
	
	available_cook = Ncook;
	available_oven = Noven;
	available_deliverer = Ndeliverer;
	available_packer = Npacker;

	printf("\nYou chose %d orders and seed = %d \n\n", number_of_orders, seed);

	int order_id[number_of_orders];
	for (int i=0; i<number_of_orders; ++i) {
		order_id[i]=i;
	}

	// create threads
	int random_time;
	pthread_t tid[number_of_orders];
	for (int i=0; i<number_of_orders; ++i) {
		random_time = rand_r(&seed)% (Torderhigh+1);
		if ( random_time < Torderlow){
			random_time = Torderlow;
		}
		pthread_create(&tid[i], NULL, &order_execution, &order_id[i]);
		sleep(random_time);
	}
	
	// wait until all the threads are done
	for (int i=0; i<number_of_orders; ++i) {
		pthread_join(tid[i], NULL);
	}
	
	printf("\n");
	printf("\nThe total income from sales is: %d euros\n", income);
	printf("\nThe number of plain orders is: %d. \n", plain);
    printf("\nThe number of special orders is: %d.\n", special);
	printf("\nThe number of failed orders is: %d. \n", failed_orders);
	printf("\nThe number of successful orders is: %d.\n", successful_orders);
	//if there was at least one successful order
	if (successful_orders>0){
		printf("\nThe average customer service time is: %.3f minutes\n", total_delivery_time / successful_orders);
		printf("\nThe maximum customer service time is: %.3f minutes\n\n", max_delivery_time);
		printf("\nAverage order cooling time is: %.3f minutes\n", total_cooling_time / successful_orders);
		printf("\nThe maximum cooling time for orders is: %.3f minutes\n\n", max_cooling_time);
	}

	return 0;
}