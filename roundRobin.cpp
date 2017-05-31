/*В этом задании вам потребуется реализовать планировщик, использующий
алгоритм Round Robin.
Реализация планировщика состоит из нескольких функций:

    void scheduler_setup(int timeslice) - вызывается перед началом работы,
    а timeslice - квант времени, который нужно использовать в некоторых единицах
    времени (что именно используется как единица времени, не существенно);
    
    void new_thread(int thread_id) - оповещает планировщик о новом потоке
    с идентификатором thread_id;
    
    void exit_thread() - оповещает планировщик о том, что текущий исполняемый
    на CPU поток завершился (соответственно, планировщик должен отдать
    CPU кому-то другому);
    
    void block_thread() - оповещает планировщик, что текущий исполняемый поток
    был заблокирован (например, запросил IO операцию и должен отдать CPU);
    
    void wake_thread(int thread_id) - оповещает, что поток с идентификатором 
    thread_id был разблокирован (например, IO операция завершилась);
    
    void timer_tick() - вызывается через равные интервалы времени, нотифицирует,
    что прошла одна единица времени;
    
    int current_thread(void) - функция должна возвращать идентификатор потока,
    который сейчас должен выполняться на CPU, если такого потока нет,
    то нужно вернуть -1.

При выполнении задания каждый раз, когда поток выполняется на CPU
и вызывается timer_tick, считайте, что поток отработал целую единицу
времени на CPU. Т. е. даже если предыдущий поток добровольно освободил
CPU (вызвав block_thread или exit_thread) и сразу после того, как CPU
был отдан другому потоку, была вызвана функция timer_tick, то все равно
считается, что второй поток отработал целую единицу времени на CPU.*/


#include <deque>


#define MY_TEST 1
#define MY_SPY 1
#define OSTREAM std::cerr
#define TEST_NUM 1

#ifdef MY_TEST
  #include <iostream>
#endif  

////////////////////////////////////////////////////////////////
// Global definitions

// ID of undefined process.
#define UNDEF_PID -1

// All globals variables is here
struct SchedulerDescription {
public:
	SchedulerDescription(): _timeslice(1), _currendPID(UNDEF_PID), _time(0)
	#ifdef MY_SPY
		, testCount(0)
	#endif
	{};
	
	// Type of process ID.
	typedef int ProcessID;
	
	// Type of queue of processes ids. First process in a queue is next to cpu.
	typedef std::deque<ProcessID> IDQueue;

	// Set variables of the object to initial values.
	void Init(int timeslice) {
		#ifdef MY_SPY
			++testCount;
			std::cerr << "Init(" << timeslice << ") #" << testCount << std::endl;
		#endif
		_pqueue.clear();
		_timeslice = timeslice;
		_currendPID = UNDEF_PID;
		_time = 0; // First Tick() will just call Tock().
	};

	// Return current working PID
	ProcessID workingPID() {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				std::cerr << "   workingPID() = " << (int)_currendPID << std::endl;
		#endif
		return _currendPID;
	};
	
	// Return time since the last Tick()
	int time() { return _time; };
	
	// Insert PID to the queue if there is current process.
	// Otherwise pid became current process.
	void AddProcess(ProcessID pid) {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   AddProcess " << (int)pid << std::endl;
		#endif
		
		
		if(_currendPID == UNDEF_PID)
			ActivateProcess(pid);
		else
			_pqueue.push_back(pid);
		
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				ShowQueue();
		#endif
	}
	
	// Add current PID to the queue.
	// Set fist PID in queue as working.
	// Shift processes in the queue.
	void SwitchNextProcess() {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   SwitchNextProcess" << std::endl;
		#endif
		
		if(_pqueue.empty()) {
			#ifdef MY_TEST
				if(testCount == TEST_NUM)
					ShowQueue();
			#endif
			// It does not make sense to switch with empty queue.
			return;
		}
		
		if(_currendPID != UNDEF_PID)
			_pqueue.push_back(_currendPID);
		
		ActivateProcess(_pqueue.front());
		_pqueue.pop_front();
		
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				ShowQueue();
		#endif
	};
	
	// Clear current PID.
	// Switch to next PID.
	void KillCurrentProcess() {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   KillCurrentProcess" << std::endl;
		#endif
		
		// Clear PID of the current process
		ActivateProcess(UNDEF_PID);
		SwitchNextProcess();
	};
	
	
	// Add one to the timer. It time == timeslice, call Tock() function.
	void Tick() {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   Tick ";
		#endif
		
		if(_currendPID == UNDEF_PID) {
			#ifdef MY_TEST
				if(testCount == TEST_NUM)
					OSTREAM << " did not do. There is not process on CPU." << std::endl;
			#endif
			return;
		}
		
		++_time;
		
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << _time << std::endl;
		#endif
		
		if(_time == _timeslice) {
			_time = 0;
			Tock();
		}
	}
	
	void ShowQueue() {
		OSTREAM << "      - State of sheduler. Current ID " << _currendPID << std::endl;
		OSTREAM << "        Queue (back to front) ";
		if(_pqueue.empty())
			OSTREAM << "is empty.";
		for(auto i=_pqueue.crbegin(); i!=_pqueue.crend(); ++i)
			OSTREAM << *i << ", ";
		OSTREAM << std::endl;
	}
	
private:
	// Called when timeslice ticks elapsed sice last Tock().
	void Tock() {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   Tock" << std::endl;
		#endif
		
		SwitchNextProcess();
	}
	
	void ActivateProcess(ProcessID pid) {
		#ifdef MY_TEST
			if(testCount == TEST_NUM)
				OSTREAM << "   ActivateProcess" << pid << std::endl;
		#endif
		
		_currendPID = pid;
		_time = 0; // Start count from the beginning
	}
	
	// Queue of processes ids
	IDQueue _pqueue;
	
	// Const from scheduler_setup().
	int _timeslice;
	
	// ID of the working process
	ProcessID _currendPID;
	
	// Timer counter. It takes values from the [0; _timeslice-1] interval.
	int _time;
	
	#ifdef MY_SPY
		int testCount;
	#endif
	
} __scheduler;


////////////////////////////////////////////////////////////////
/**
* Функция будет вызвана перед каждым тестом, если вы
* используете глобальные и/или статические переменные
* не полагайтесь на то, что они заполнены 0 - в них
* могут храниться значения оставшиеся от предыдущих
* тестов.
*
* timeslice - квант времени, который нужно использовать.
* Поток смещается с CPU, если пока он занимал CPU функция
* timer_tick была вызвана timeslice раз.
**/
void scheduler_setup(int timeslice) {
	#ifdef MY_SPY
		std::cerr << "scheduler_setup(" << timeslice << ")" << std::endl;
	#endif
    __scheduler.Init(timeslice);
}

/**
 * Функция вызывается, когда создается новый поток управления.
 * thread_id - идентификатор этого потока и гарантируется, что
 * никакие два потока не могут иметь одинаковый идентификатор.
 **/
void new_thread(int thread_id) {
	#ifdef MY_SPY
		std::cerr << "new_thread(" << thread_id << ")" << std::endl;
	#endif
    __scheduler.AddProcess(thread_id);
}

/**
 * Функция вызывается, когда поток, исполняющийся на CPU,
 * завершается. Завершится может только поток, который сейчас
 * исполняется, поэтому thread_id не передается. CPU должен
 * быть отдан другому потоку, если есть активный
 * (незаблокированный и незавершившийся) поток.
 **/
void exit_thread() {
	#ifdef MY_SPY
		std::cerr << "exit_thread()" << std::endl;
	#endif
    __scheduler.KillCurrentProcess();
}

/**
 * Функция вызывается, когда поток, исполняющийся на CPU,
 * блокируется. Заблокироваться может только поток, который
 * сейчас исполняется, поэтому thread_id не передается. CPU
 * должен быть отдан другому активному потоку, если таковой
 * имеется.
 **/
void block_thread() {
	#ifdef MY_SPY
		std::cerr << "block_thread()" << std::endl;
	#endif
    __scheduler.KillCurrentProcess();
}

/**
 * Функция вызывается, когда один из заблокированных потоков
 * разблокируется. Гарантируется, что thread_id - идентификатор
 * ранее заблокированного потока.
 **/
void wake_thread(int thread_id) {
	#ifdef MY_SPY
		std::cerr << "wake_thread(" << thread_id << ")" << std::endl;
	#endif
    __scheduler.AddProcess(thread_id);
}

/**
 * Ваш таймер. Вызывается каждый раз, когда проходит единица
 * времени.
 **/
void timer_tick() {
	#ifdef MY_SPY
		std::cerr << "timer_tick()" << std::endl;
	#endif
    __scheduler.Tick();
}

/**
 * Функция должна возвращать идентификатор потока, который в
 * данный момент занимает CPU, или -1 если такого потока нет.
 * Единственная ситуация, когда функция может вернуть -1, это
 * когда нет ни одного активного потока (все созданные потоки
 * либо уже завершены, либо заблокированы).
 **/
int current_thread() {
	#ifdef MY_SPY
		std::cerr << "current_thread()" << std::endl;
	#endif
	
    return __scheduler.workingPID();
}

#ifdef MY_TEST

void test1() {
  OSTREAM << "Init scheduler" << std::endl;
  scheduler_setup(10);

  new_thread(1);
  new_thread(2);
  new_thread(3);

  OSTREAM << "current thread: " << current_thread() << std::endl;

  for (int i = 0; i < 4; i++){
    timer_tick();
    OSTREAM << "current thread: " << current_thread() << std::endl;
  }
  int block = current_thread();
  OSTREAM << "block thread: " << block << std::endl;
  block_thread();
  OSTREAM << "current thread: " << current_thread() << std::endl;
  for (int i = 0; i < 4; i++){
    timer_tick();
    OSTREAM << "current thread: " << current_thread() << std::endl;
  }
  wake_thread(block);
  OSTREAM << "wake thread: " << block << std::endl;
  OSTREAM << "current thread: " << current_thread() << std::endl;
  for (int i = 0; i < 4; i++){
    timer_tick();
    OSTREAM << "current thread: " << current_thread() << std::endl;
  }
  OSTREAM << "exit thread: " << current_thread() << std::endl;
  exit_thread();
  OSTREAM << "current thread: " << current_thread() << std::endl;
  for (int i = 0; i < 4; i++){
    timer_tick();
    OSTREAM << "current thread: " << current_thread() << std::endl;
  }
  OSTREAM << "exit thread: " << current_thread() << std::endl;
  exit_thread();
  OSTREAM << "current thread: " << current_thread() << std::endl;
  for (int i = 0; i < 4; i++){
    timer_tick();
    OSTREAM << "current thread: " << current_thread() << std::endl;
  }
  OSTREAM << "exit thread: " << current_thread() << std::endl;
  exit_thread();
  OSTREAM << "current thread: " << current_thread() << std::endl;
}

void test2(){
	scheduler_setup(2);
	current_thread();
	current_thread();
	current_thread();
	new_thread(0);
	current_thread();
	current_thread();
	block_thread();
	current_thread();
	timer_tick();
	current_thread();
	current_thread();
	new_thread(1);
	current_thread();
	current_thread();
	block_thread();
	current_thread();
	timer_tick();
	current_thread();
	wake_thread(0);
	current_thread();
	new_thread(2);
	current_thread();
	current_thread();
	block_thread();
	current_thread();
	timer_tick();
	current_thread();
	wake_thread(1);
	current_thread();
	new_thread(3);
	current_thread();
	current_thread();
	block_thread();
	current_thread();
	timer_tick();
	current_thread();
}

int main() {
  //test1();
  test2();
  
  return 0;
}

#endif