
#include "semaphore.h"


using namespace std;

int id = 0; // id
int p_index = 0 ;// process_index

struct Menu
{
 int id;
 char name[15] ; 
 double price;
 int totalOrders;
};

struct OrdersBoard
{
 int itemId;
 int amount;
 bool done;
};

//--------------------------------------------------------FUNC DECLARATION-----------------------------------------------------
void input_check(int argc, char* argv[], int& time_of_simulation, int& items_amount , int& customers_amount, int& waiters_amount);//done
double timeElapsed(std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start);//done
double randomSleep(double min , double max);
int sum_of_orders(Menu* menu , int num_of_items , double& totalPrice);

//menu functions
void share_the_menu(Menu** menu); //done
void fill_the_menu(Menu* menu); //done
void add_to_menu(int id, string item_name, double price, Menu* menu); //done
void print_the_menu(Menu* menu, int items_amount);//done
void share_the_count_reader_menu(int** menu_readers);
void read_the_Menu(int* menu_readers_count, int serviceQueue2 , int menuReadersCountAccess , int menuAccess , int num_of_items , int CustomerId ,Menu* menu, OrdersBoard* orders );//done
void write_to_menu(int serviceQueue2 ,int mutex_print , int menuAccess , int num_of_items , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int id , int itemId ,Menu* menu , OrdersBoard* orders , int amount , int custId); 

//orders board functions
void share_the_orders_board(OrdersBoard** ob); //done
void initialize_the_orders_board(OrdersBoard** ob,int customers_amount); //done
void share_the_count_reader_order(int** order_readers);//done
void write_item_to_board(int CustomerId, int ItemId, int Amount, OrdersBoard* orders );//done
void make_order_done(int CustomerID , bool Done, OrdersBoard* orders);//done
void read_the_order_board(string type , int* ordersReaders ,int ordersReadersCountAccess ,int ordersAccess , int serviceQueue , int num_of_customers , int CustomerId ,Menu* menu , OrdersBoard* orders , int& amount , int& custId , int& itemId , bool& flag);//done
void write_order_to_board(string type ,int serviceQueue , int mutex_print , int ordersAccess , int num_of_items , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int CustomerId ,Menu* menu ,OrdersBoard* orders );//done
//semaphore functions

//customer functions
void customer(double simulation_time , int* menu_readers_count ,int* ordersReaders , int serviceQueue ,int intserviceQueue2 , int menuReadersCountAccess ,int ordersReadersCountAccess , int mutex_print , int menuAccess ,int ordersAccess , int num_of_items , int num_of_customers ,std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int CustomerId ,Menu* menu, OrdersBoard* orders );

//waiter functions
void waiter(double simulation_time , int* menu_readers_count ,int* ordersReaders , int serviceQueue ,int serviceQueue2 , int menuReadersCountAccess ,int ordersReadersCountAccess , int mutex_print , int menuAccess, int ordersAccess , int num_of_items ,int num_of_customers , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int CustomerId ,Menu* menu, OrdersBoard* orders );

//-----------------------------------------------------MAIN-----------------------------------------

int main(int argc, char *argv[])
{
 auto start = std::chrono::steady_clock::now();
 int time_of_simulation; // simulation_time
 int items_amount; // num_of_items_in_menu
 int customers_amount; // num_of_customers
 int waiters_amount; // num_of_waiters

 input_check(argc, argv, time_of_simulation, items_amount, customers_amount, waiters_amount);

 //Initilaize and share the menu
 Menu* menu = NULL;
 share_the_menu(&menu);
 fill_the_menu(menu);
 cout << " " << fixed << setprecision(3) << timeElapsed(start) << " Main Process ID " << getpid() << " start" << endl;
 print_the_menu(menu, items_amount);
 cout << " " << fixed << setprecision(3) << timeElapsed(start) << " Main Process start creating sub-process" << endl;

 //Initilaize and share the orders board
 OrdersBoard* ob = NULL;
 share_the_orders_board(&ob);
 initialize_the_orders_board(&ob , customers_amount);

 //Initilaize the readers counters
 int* menu_readers_count = NULL ;
 int* orders_readers_count = NULL ; 
 share_the_count_reader_menu(&menu_readers_count);
 share_the_count_reader_order(&orders_readers_count);
 *menu_readers_count = 0 ; 
 *orders_readers_count = 0 ;
 
 //Semaphores
 int serviceQueue ,serviceQueue2 , menuReadersCountAccess ,ordersReadersCountAccess , menuAccess , ordersAccess ,mutex_print , status ;
 key_t semKey0 = ftok(".", 0);
 key_t semKey1 = ftok(".", 1);
 key_t semKey2 = ftok(".", 2);
 key_t semKey3 = ftok(".", 3);
 key_t semKey4 = ftok(".", 4);
 key_t semKey5 = ftok(".", 5);
 key_t semKey6 = ftok(".", 6);
 if( ( serviceQueue = initsem(semKey0, 1) ) < 0 )
    {
        exit(0);
    }
 if( ( serviceQueue2 = initsem(semKey1, 1) ) < 0 )
    {
        exit(0);
    }
 if( ( menuReadersCountAccess = initsem(semKey2,customers_amount+waiters_amount ) ) < 0 )
    {
        exit(0);
    }
 if( ( ordersReadersCountAccess = initsem(semKey3,customers_amount+waiters_amount ) ) < 0 )
    {
        exit(0);
    }
 if( ( menuAccess = initsem(semKey4,1 ) ) < 0 )
    {
        exit(0);
    }
 if( ( ordersAccess = initsem(semKey5,1 ) ) < 0 )
    {
        exit(0);
    }
 if( ( mutex_print = initsem(semKey6,1 ) ) < 0 )
    {
        exit(0);
    }
 
 pid_t pid=-1;
 int id= -1; 
 string ptype = "";
 //creates waiters process
 for (int i = 0; i < waiters_amount ; i++)
 {   
 	if(pid!=0){
 	if ((pid = fork()) < 0)
 	{
 		cout << "ERROR: forking child process failed" << endl;
 		exit(1);
	}
 	if(pid==0){
 		id =i ; 
 		ptype.assign("Waiter") ;
		} 
 	}
 }

 //creates customer process
 for (int j = 0; j < customers_amount ; j++)
 {
 	if(pid!=0){
 	if ((pid = fork()) < 0)
 	{
 		cout << "ERROR: forking child process failed" << endl;
 		exit(1);
 	}
 	if(pid==0){
 		id=j;
 		ptype.assign("Customer");
 		}
 	}
 }

 if(pid==0){	
 	srand(id*time(0));
 	if(ptype == "Customer"){
 		down(mutex_print);
 		cout <<" " << fixed << setprecision(3) <<timeElapsed(start) << " Customer " << id << ": created PID " << getpid() << " " << " PPID " << getppid() <<endl ; 
 		up(mutex_print);
 		customer(time_of_simulation , menu_readers_count ,orders_readers_count, serviceQueue ,serviceQueue2 , menuReadersCountAccess , ordersReadersCountAccess , mutex_print , menuAccess ,ordersAccess, items_amount , customers_amount ,start , id , menu, ob );
 		}
 	else if(ptype=="Waiter"){
 		down(mutex_print); 
 		cout <<" " << fixed << setprecision(3) <<timeElapsed(start) << " Waiter " << id << ": created PID " << getpid() << " " << " PPID " << getppid() <<endl ; 
 		up(mutex_print); 
		waiter(time_of_simulation , menu_readers_count ,orders_readers_count, serviceQueue , serviceQueue2 , menuReadersCountAccess ,ordersReadersCountAccess ,mutex_print , menuAccess , ordersAccess , items_amount ,customers_amount , start , id , menu, ob );
 		}
 		down(mutex_print);
 		cout <<" " <<fixed << setprecision(3) << timeElapsed(start) <<" " << ptype << " ID " <<id << ": PID "<< getpid() << " end work PPID "<<getppid() <<endl ; 
 		up(mutex_print);
 		}

 else { //father of all processes
 	while( waitpid(-1, &status , 0) != -1 );
 	print_the_menu(menu,items_amount);
 	double totalPrice ; 
 	cout << " Total orders " << sum_of_orders(menu , items_amount , totalPrice) ; 
 	cout <<" , for an amount " <<fixed << setprecision(2)<< totalPrice << " NIS" <<endl ;
 	cout <<" " <<fixed << setprecision(3) << timeElapsed(start) <<" MAIN ID " << getpid() << " end work" <<endl ; 
 	cout <<" " <<fixed << setprecision(3) << timeElapsed(start) <<" END of simulation" <<endl ;
 } 
 return 0 ;
}

//-----------------------------------------------FUNC IMPLEMENTATION------------------------------------------


void input_check(int argc, char* argv[], int& time_of_simulation, int& items_amount , int& customers_amount, int& waiters_amount)
{
 bool check_args = true;
 if (argc >= 5)
 {
 	time_of_simulation = atoi(argv[1]);
 	items_amount = atoi(argv[2]);
 	customers_amount = atoi(argv[3]);
 	waiters_amount = atoi(argv[4]);

 	if(time_of_simulation<=0 || time_of_simulation>30|| items_amount<=0 || items_amount>10 || waiters_amount<=0 || waiters_amount>3 || 		  customers_amount<=0 || customers_amount>10)
 	{
 		check_args = false;
 	}
 }
 else 
	check_args=false ; 

 if (check_args == false)
 {
 	cout << "Input arguments are not valid!" << endl;
 	exit(1); 
 }
 else
 {
 	cout << "======Simulation Argument======" << endl;
 	cout << "Simulation time: " << time_of_simulation << endl;
 	cout << "Menu items count: " << items_amount << endl;
 	cout << "Customers count: " << customers_amount << endl;
 	cout << "Waiters count: " << waiters_amount << endl;
 	cout << "===============================" << endl;
 }
}

void share_the_menu(Menu** menu)
{
 key_t key;
 int shmid;
 // Create the key
 key = ftok(".", 'R');
 if (key == -1)
 {
 	perror("ftok");
 	exit(1);
 }
 //Create the shared memory
 shmid = shmget(key, sizeof(*menu), IPC_CREAT | 0666);
 if ( shmid< 0)
 { 
 	perror("shmget");
 	exit(1);
 }
 //Attach the menu to the shared memory  
 *menu = (Menu *)shmat(shmid, NULL, 0);
 if (*menu == (void *)-1)
 {
 	perror("shmat");
 	exit(1);
 }
}

void fill_the_menu(Menu* menu)
{
 add_to_menu(0, "Milkshake", 16.00, menu);
 add_to_menu(1, "Cake", 12.00, menu);
 add_to_menu(2, "Hamburger", 22.30, menu);
 add_to_menu(3, "Spaghetti", 19.90, menu);
 add_to_menu(4, "Salad", 15.50, menu);
 add_to_menu(5, "Pie", 10.50, menu);
 add_to_menu(6, "Pizza", 14.00, menu);
 add_to_menu(7, "Orange", 7.00, menu);
 add_to_menu(8, "Tea", 6.50, menu);
 add_to_menu(9, "Coffe", 5.50, menu);
}

void add_to_menu(int id, string item_name, double price, Menu* menu)
{
 menu[id].id = id;
 strcpy(menu[id].name, item_name.c_str());
 menu[id].price = price;
 menu[id].totalOrders = 0;
}


double timeElapsed(std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start)
{
 auto end = std::chrono::steady_clock::now();
 auto diff = end - start;
 return std::chrono::duration<double, std::milli>(diff).count() / 1000;
}

void print_the_menu(Menu* menu, int items_amount)
{
 cout << "===============Menu list===============" << endl;
 cout << setw(0) << "Id"<< setw(8) << "Name" << setw(12) << "Price" <<setw(11) << "Orders" << endl ;
 for (int i = 0; i < items_amount; i++)
 {
	cout<<menu[i].id<<"     "<<left<<setw(10)<<menu[i].name<<" "<<setw(10)<<menu[i].price<<setw(10)<<menu[i].totalOrders<<endl;
 }
 cout << "=======================================" << endl;
}

void share_the_orders_board(OrdersBoard** ob)
{
 key_t key;
 int shmid;
 // Create the key
 key = ftok(".", 'S');
 if (key == -1)
 {
 	perror("ftok");
 	exit(1);
 }
 //Create the shared memory
 shmid = shmget(key, sizeof(*ob), IPC_CREAT | 0666);
 if ( shmid< 0)
 { 
 	perror("shmget");
 	exit(1);
 }
 //Attach the menu to the shared memory  
 *ob = (OrdersBoard*)shmat(shmid, NULL, 0);
 if (*ob == (void *)-1)
 {
 	perror("shmat");
 	exit(1);
 }
}

void initialize_the_orders_board(OrdersBoard** ob,int customers_amount)
{
 for(int i = 0 ; i<customers_amount ; i++)
 	(*ob)[i].done = true ;
}

void share_the_count_reader_menu(int** menu_readers)
{
 key_t key;
 int shmid;
 key = ftok(".", 'm');
 if (key == -1)
 {
 	perror("ftok");
 	exit(1);
 }
 shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
 if (shmid < 0)
 {
 	perror("shmget");
 	exit(1);
 }
 *menu_readers = (int*)shmat(shmid, NULL, 0);
 if (*menu_readers == (void *)-1)
 {
 	perror("shmat");
 	exit(1);
 }
}


void share_the_count_reader_order(int** menu_readers)
{
 key_t key;
 int shmid;
 key = ftok(".", 'o');
 if (key == -1)
 {
 	perror("ftok");
 	exit(1);
 }
 shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
 if (shmid < 0)
 {
 	perror("shmget");
 	exit(1);
 }
 *menu_readers = (int*)shmat(shmid, NULL, 0);
 if (*menu_readers == (void *)-1)
 {
 	perror("shmat");
 	exit(1);
 }
}




 void customer(double simulation_time , int* menu_readers_count ,int* ordersReaders, int serviceQueue , int serviceQueue2 , int menuReadersCountAccess , int ordersReadersCountAccess ,int mutex_print , int menuAccess , int ordersAccess ,int items_amount ,int customers_amount , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int CustomerId ,Menu* menu, OrdersBoard* orders ){
 
 while(simulation_time>timeElapsed(start)){
 	sleep(randomSleep(3,6));
 	read_the_Menu( menu_readers_count, serviceQueue2 , menuReadersCountAccess , menuAccess , items_amount , CustomerId , menu, orders );
 	int temp ; 
 	bool orderDone = false ;
 	read_the_order_board("Customer" ,ordersReaders , ordersReadersCountAccess ,ordersAccess ,serviceQueue, -1 , CustomerId ,menu , orders ,temp , temp , temp , orderDone);
 
 	if(orderDone==true){
 		int make_order = rand()%2 ; 
 		if(make_order==1){//customer want to order
 			write_order_to_board("Customer" ,serviceQueue , mutex_print , ordersAccess , items_amount , start , CustomerId ,menu , orders);
 		}
 	else{
 		int selectedItem = rand()%(items_amount+1) ;
 		down(mutex_print);
 		printf(" %.3lf Customer %d: reads a menu about %s (doesn't want to order)\n", timeElapsed(start) , CustomerId , menu[selectedItem].name); 
 		up(mutex_print);
 		}
 	}
 }
}

// like void reader()
void read_the_Menu( int* menu_readers_count , int serviceQueue2 , int menuReadersCountAccess , int menuAccess , int items_amount , int CustomerId ,Menu* menu , OrdersBoard* orders)
{
 auto start_read = std::chrono::steady_clock::now();
 down(serviceQueue2);
 down(menuReadersCountAccess);
 if((menu_readers_count)==0)
 {
 	down(menuAccess);
  }
 (*menu_readers_count)++ ; 
 up(serviceQueue2);
 up(menuReadersCountAccess);
 //read menu for one second
 while(timeElapsed(start_read)<=1);
 
 down(menuReadersCountAccess);
 (*menu_readers_count)-- ; 
 if((*menu_readers_count)==0){
 	up(menuAccess);
  }
 up(menuReadersCountAccess);
}

void read_the_order_board(string type , int* ordersReaders ,int ordersReadersCountAccess ,int serviceQueue , int ordersAccess , int num_of_customers , int id ,Menu* menu , OrdersBoard* orders , int& amount , int& custId , int& itemId , bool& flag)
{
 down(serviceQueue);
 down(ordersReadersCountAccess);
 if(*ordersReaders==1)
 	down(ordersAccess);
 (*ordersReaders)++ ; 
 
 up(serviceQueue);
 up(ordersReadersCountAccess);
 if(type=="Customer"){ 
 	if(orders[id].done == true) {
 		flag= true ; //order is done
 	}
 	else {
 		flag= false ; //order is not done 
 		}
 }
 else{//waiter
 	flag = false ; //there is no more orders
 for(int i = 0 ; i<num_of_customers ; i++)
  {
 	if(orders[i].done==false){
 	itemId = orders[i].itemId ; 
 	amount = orders[id].amount ;
 	custId = i ; 
 	flag= true ;//has more orders 
 	break;
  }
 }
}
 down(ordersReadersCountAccess);
 (*ordersReaders)-- ; 
 if(*ordersReaders==0)
 up(ordersAccess);
 up(ordersReadersCountAccess);
}



double randomSleep(double min , double max){
 double f = (double)rand()/RAND_MAX ; 
 return min + f*(max-min) ; 
}

// like void writer()
void write_order_to_board(string type ,int serviceQueue, int outAccess , int ordersAccess , int num_of_items , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int id ,Menu* menu , OrdersBoard* orders ){
 int selectedItem = rand()%(num_of_items+1) ;
 double elapsedTime ; 
 int amount ; 
 down(serviceQueue);
 down(ordersAccess);
 up(serviceQueue);
 if(type=="Customer"){
 	amount = rand()%4 +1 ;
 	elapsedTime = timeElapsed(start); 
 	write_item_to_board(id, selectedItem , amount, orders );
 	down(outAccess);
 	printf(" %.3lf Customer %d: reads a menu about %s (ordered, amount %d)\n", elapsedTime , id , menu[selectedItem].name , amount); 
 	up(outAccess); 
 }
 else{//waiter
 	make_order_done(id,true ,orders); 
 }
 up(ordersAccess);
}


void write_item_to_board(int CustomerId, int itemId, int amount, OrdersBoard* orders )
{
 orders[CustomerId].itemId = itemId;
 orders[CustomerId].amount = amount;
 orders[CustomerId].done = false; 
}

void make_order_done(int CustomerID , bool done, OrdersBoard* orders)
{
 orders[CustomerID].done = done ; 
}

void waiter(double simulation_time , int* menuReaders ,int* ordersReaders, int serviceQueue ,int serviceQueue2 , int menuReadersCountAccess ,int ordersReadersCountAccess , int outAccess , int menuAccess, int ordersAccess , int num_of_items ,int num_of_customers , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int CustomerId ,Menu* menu, OrdersBoard* orders ){
 while(simulation_time>timeElapsed(start)){
 	sleep(randomSleep(1,2));
 	int amount ; 
 	int custId ; 
 	int itemId; 
	bool not_perform = false ; 
 	read_the_order_board("Waiter" ,ordersReaders , ordersReadersCountAccess ,ordersAccess,serviceQueue, num_of_customers , id ,menu , orders , amount , custId , itemId , not_perform);//if there is more orders
 	if(not_perform==true){
 		write_order_to_board("Waiter" , serviceQueue, outAccess , ordersAccess, num_of_items ,start, custId ,menu , orders);
 		write_to_menu(serviceQueue2 , outAccess , menuAccess , num_of_items , start , id , itemId ,menu , orders , amount , custId);
 
 }
 }
}



void write_to_menu(int serviceQueue2 ,int outAccess , int menuAccess , int num_of_items , std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > start , int id , int itemId ,Menu* menu , OrdersBoard* orders , int amount , int custId){ 
 down(serviceQueue2);
 down(menuAccess); 
 up(serviceQueue2);
 menu[itemId].totalOrders = menu[itemId].totalOrders + amount ; 
 down(outAccess);
 printf(" %.3lf Waiter %d: performs the order of customer ID %d (%d %s)\n" , timeElapsed(start) ,id ,custId , amount , menu[itemId].name );
 up(outAccess);
 up(menuAccess);
}


int sum_of_orders(Menu* menu , int num_of_items , double& totalPrice){
 int TotalOrders = 0 ;
 totalPrice = 0 ; 
 for(int i = 0 ; i<num_of_items ; i++ ){
 TotalOrders += menu[i].totalOrders ; 
 totalPrice += menu[i].totalOrders*menu[i].price ; 
 }
 return TotalOrders ;
}





