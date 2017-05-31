/*� ���� ������� ��� ����������� ����������� �����������, ������������
�������� Round Robin.
���������� ������������ ������� �� ���������� �������:

    void scheduler_setup(int timeslice) - ���������� ����� ������� ������,
    � timeslice - ����� �������, ������� ����� ������������ � ��������� ��������
    ������� (��� ������ ������������ ��� ������� �������, �� �����������);
    
    void new_thread(int thread_id) - ��������� ����������� � ����� ������
    � ��������������� thread_id;
    
    void exit_thread() - ��������� ����������� � ���, ��� ������� �����������
    �� CPU ����� ���������� (��������������, ����������� ������ ������
    CPU ����-�� �������);
    
    void block_thread() - ��������� �����������, ��� ������� ����������� �����
    ��� ������������ (��������, �������� IO �������� � ������ ������ CPU);
    
    void wake_thread(int thread_id) - ���������, ��� ����� � ��������������� 
    thread_id ��� ������������� (��������, IO �������� �����������);
    
    void timer_tick() - ���������� ����� ������ ��������� �������, ������������,
    ��� ������ ���� ������� �������;
    
    int current_thread(void) - ������� ������ ���������� ������������� ������,
    ������� ������ ������ ����������� �� CPU, ���� ������ ������ ���,
    �� ����� ������� -1.

��� ���������� ������� ������ ���, ����� ����� ����������� �� CPU
� ���������� timer_tick, ��������, ��� ����� ��������� ����� �������
������� �� CPU. �. �. ���� ���� ���������� ����� ����������� ���������
CPU (������ block_thread ��� exit_thread) � ����� ����� ����, ��� CPU
��� ����� ������� ������, ���� ������� ������� timer_tick, �� ��� �����
���������, ��� ������ ����� ��������� ����� ������� ������� �� CPU.*/


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
* ������� ����� ������� ����� ������ ������, ���� ��
* ����������� ���������� �/��� ����������� ����������
* �� ����������� �� ��, ��� ��� ��������� 0 - � ���
* ����� ��������� �������� ���������� �� ����������
* ������.
*
* timeslice - ����� �������, ������� ����� ������������.
* ����� ��������� � CPU, ���� ���� �� ������� CPU �������
* timer_tick ���� ������� timeslice ���.
**/
void scheduler_setup(int timeslice) {
	#ifdef MY_SPY
		std::cerr << "scheduler_setup(" << timeslice << ")" << std::endl;
	#endif
    __scheduler.Init(timeslice);
}

/**
 * ������� ����������, ����� ��������� ����� ����� ����������.
 * thread_id - ������������� ����� ������ � �������������, ���
 * ������� ��� ������ �� ����� ����� ���������� �������������.
 **/
void new_thread(int thread_id) {
	#ifdef MY_SPY
		std::cerr << "new_thread(" << thread_id << ")" << std::endl;
	#endif
    __scheduler.AddProcess(thread_id);
}

/**
 * ������� ����������, ����� �����, ������������� �� CPU,
 * �����������. ���������� ����� ������ �����, ������� ������
 * �����������, ������� thread_id �� ����������. CPU ������
 * ���� ����� ������� ������, ���� ���� ��������
 * (����������������� � ���������������) �����.
 **/
void exit_thread() {
	#ifdef MY_SPY
		std::cerr << "exit_thread()" << std::endl;
	#endif
    __scheduler.KillCurrentProcess();
}

/**
 * ������� ����������, ����� �����, ������������� �� CPU,
 * �����������. ��������������� ����� ������ �����, �������
 * ������ �����������, ������� thread_id �� ����������. CPU
 * ������ ���� ����� ������� ��������� ������, ���� �������
 * �������.
 **/
void block_thread() {
	#ifdef MY_SPY
		std::cerr << "block_thread()" << std::endl;
	#endif
    __scheduler.KillCurrentProcess();
}

/**
 * ������� ����������, ����� ���� �� ��������������� �������
 * ��������������. �������������, ��� thread_id - �������������
 * ����� ���������������� ������.
 **/
void wake_thread(int thread_id) {
	#ifdef MY_SPY
		std::cerr << "wake_thread(" << thread_id << ")" << std::endl;
	#endif
    __scheduler.AddProcess(thread_id);
}

/**
 * ��� ������. ���������� ������ ���, ����� �������� �������
 * �������.
 **/
void timer_tick() {
	#ifdef MY_SPY
		std::cerr << "timer_tick()" << std::endl;
	#endif
    __scheduler.Tick();
}

/**
 * ������� ������ ���������� ������������� ������, ������� �
 * ������ ������ �������� CPU, ��� -1 ���� ������ ������ ���.
 * ������������ ��������, ����� ������� ����� ������� -1, ���
 * ����� ��� �� ������ ��������� ������ (��� ��������� ������
 * ���� ��� ���������, ���� �������������).
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