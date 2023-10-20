#include<iostream>
#include<thread>
#include<Windows.h>
#include<chrono>


using namespace std;
using namespace std::literals::chrono_literals;

enum Color
{
	Black, Blue, Green, Cyan, Red, Magenta, Brown,
	LightGray, DarkGray, LightBlue, LightGreen, LightCyan, LightRed, LightMagenta, Yellow, White
};

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(Color text, Color background)
{
	SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

enum Direction { Up = 72, Left = 75, Right = 77, Down = 80, Spase = 32, Enter = 13, esc = 27 };

void setCursor(int x, int y)
{
	COORD myCoords = { x,y };//инициализируем передаваемыми значениями объект координат
	SetConsoleCursorPosition(hStdOut, myCoords);
}


#define MIN_TANK_VOLUME 40
#define MAX_TANK_VOLUME 200
#define MIN_TANK_LEVEL 5

class Tank 
{
	const int VOLUME;
	double  fuel_level;
public:
	int get_VOLUME() const
	{
		return VOLUME;
	}
	double get_fuel_level() const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0)return;
		if (fuel + this->fuel_level > VOLUME)this->fuel_level = VOLUME;
		else this->fuel_level += fuel;
	}
	double cosuption_engine(double amount)   //расход топлива
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	void info() const
	{
		cout << "Объем:\t" << get_VOLUME() << " литров.\n";
		cout << "Топлива:\t" << get_fuel_level() << " Литров.\n";

	}
	bool is_fill()
	{
		if (get_fuel_level() > MIN_TANK_LEVEL && get_fuel_level() < VOLUME) return true;
		else return false;
	}


	Tank(int volume) : VOLUME(
		volume<MIN_TANK_VOLUME? MIN_TANK_VOLUME :
		volume>MAX_TANK_VOLUME? MAX_TANK_VOLUME :
		volume), fuel_level(0){}
	~Tank(){}
};

#define MIN_ENGINE_CONSUMPTION 20   //на 100км
#define MAX_ENGINE_CONSUMPTION 200   

class Engine
{
	const double CONSUMPTION;
	double consumption_per_second;
	bool is_started;
public:
	void set_consuption_per_second()
	{
		consumption_per_second = CONSUMPTION * 3e-5;
	}
	double get_consuption_per_second()const
	{
		return consumption_per_second;
	}
	double expenditure_consumption_per_second(int speed)
	{
			speed == 0 ?  consumption_per_second = 0.0003 :
			speed < 60 ?  consumption_per_second = 0.0014 :
			speed < 100 ? consumption_per_second = 0.0020 :
			speed < 140 ? consumption_per_second = 0.0020 :
			speed < 200 ? consumption_per_second = 0.0025 :
			speed < 250 ? consumption_per_second = 0.0030 :
			get_consuption_per_second();

			return consumption_per_second;
	}
	double get_consuption()const
	{
		return CONSUMPTION;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()
	{
		return is_started;
	}
	void Info()const
	{
		cout << "Расход на 100 км.:  " << CONSUMPTION << " л." << endl;
		cout << "Расход за 1 секунду: " << consumption_per_second << " л." << endl;
		cout << "Двигатель " << (is_started ? "ON" : "OFF") << endl;
	}
	Engine(double consumption) :CONSUMPTION(
		consumption<MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
		consumption>MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
		consumption)
	{
		set_consuption_per_second();
		is_started = false;
	}
	~Engine(){}
};

#define MAX_SPEED_MAX 320
#define MAX_SPEED_MIN 140
#define MAX_ACCELERATION 40
#define MIN_ACCELERATION 25
#define MIN_CONSUMPTION 0,003

class Car
{
	Tank tank;
	Engine engine;

	int speed;
	const int MAX_SPEED;
	const int ACCELERATION;
	bool driver_inside;

	struct
	{
		std::thread panel_thread;
		std::thread idle_thread;
		std::thread friction_force;
	}threads;
public:
	int get_speed()
	{
		return speed;
	}
	void driver_in()
	{
		driver_inside = true;
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void driver_out()
	{
		system("cls");
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		cout << "Вы вышли! Чтобы заправится нажмите RContrl. Что бы зайти в машину Space" << endl;
		std::this_thread::sleep_for(1s);
	}
	void panel()
	{
		while (driver_inside)
		{
			system("cls");
			cout << "Вы в машине!" << endl;
			cout << "стрелка вверх: газ" << endl;
			cout << "стрелка вниз: торможение" << endl;
			cout << "RSHIFT: завести машину" << endl;
			cout << "RCtrl :заправить машину" << endl;
			cout << "Space: войти/выйти из машины" << endl << endl;
			if (engine.started())
			{
				cout << "Двигатель: ";
				setColor(Green, Black);
				cout << "Включен"<< endl;
				setColor(White, Black);
			}
			else
			{
				cout << "Двигатель: ";
				setColor(Red, Black);
				cout << "Выключен" << endl;
				setColor(White, Black);
			}
			cout << "Уровень топлива: " << tank.get_fuel_level() << " литров" << endl;
			cout << "Скорость: " << speed << " км/ч" << endl;
			cout << "Расход на 100 км.:  " << engine.get_consuption() << " л." << endl;
			cout << "Расход за 1 секунду: " << engine.get_consuption_per_second() << " л." << endl;

			alarm();
			std::this_thread::sleep_for(1s);
		}
	}
	void friction()
	{
		while (--speed > 0)
		{
			std::this_thread::sleep_for(2s);
			cout << "Трение" << endl;
		}
	}
	void idle_car()
	{
		while (engine.started() && tank.cosuption_engine(engine.get_consuption_per_second()))
		{
			if (tank.get_fuel_level() == 0) engine.stop();

			std::this_thread::sleep_for(1s);
		}
	}
	void move_car()
	{
			speed += ACCELERATION;
			if (speed > MAX_SPEED)speed = MAX_SPEED;
			tank.cosuption_engine(engine.expenditure_consumption_per_second(speed));
			std::this_thread::sleep_for(1s);
			threads.friction_force = std::thread(&Car::friction, this);
			if (threads.friction_force.joinable())threads.friction_force.detach();
	}
	void brake_car()
	{
			speed -= ACCELERATION*2.5;
			if (speed < 1)speed = 0;
			tank.fill(engine.get_consuption_per_second());
			std::this_thread::sleep_for(1s);
			threads.friction_force = std::thread(&Car::friction, this);
			if (threads.friction_force.joinable())threads.friction_force.detach();

	}
	void stop_car()
	{
		//speed = 0;
		engine.stop();
		if (threads.idle_thread.joinable())threads.idle_thread.join();
	}
	void start_car()//открываем поток для старта машины, чтобы списывалось топливо, пока заведена
	{
		engine.start();
		threads.idle_thread = std::thread(&Car::idle_car, this);
	}
	void alarm()
	{
		if (!tank.is_fill())
		{
			setColor(LightRed, Black);
			cout << "Низкий уровень топлива!!!!!!!" << endl;
			setColor(White, Black);
		}
	}


	void machine_control()
	{
		do
		{
			std::this_thread::sleep_for(500ms);
			if (GetAsyncKeyState(VK_UP))
			{
				if (driver_inside && engine.started())
				{
					move_car();
				}

			}
			if (GetAsyncKeyState(VK_DOWN))
			{
				if (driver_inside)
				{
					brake_car();
				}

			}
			if (GetAsyncKeyState(VK_RSHIFT))
			{
				if (driver_inside)
				{
					engine.started() ? stop_car() : start_car();
				}

			}
			if (GetAsyncKeyState(VK_RCONTROL))
			{
				if (driver_inside)
				{
					setColor(Red, Black);
					cout << "Нужно выйти из машины!!!!!!!!!!!!" << endl;
					setColor(White, Black);
				}
				if (!driver_inside)
				{
					double kol_fuel;
					cout << "Заправляю: "; cin >> kol_fuel;
					tank.fill(kol_fuel);

				}

			}
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				driver_out();
				cout << "Пока" << endl;
				break;
			}
			if (GetAsyncKeyState(VK_SPACE))
			{
				if (!engine.started()) driver_inside ? driver_out() : driver_in();
			}
			if (tank.get_fuel_level() == 0) stop_car();
			
		} while (true);
	}
	void Info()const
	{
		engine.Info();
		tank.info();
		cout << "Максимальная скорость:    \t" << MAX_SPEED << " km/h\n";
		cout << "Ускорение:\t" << ACCELERATION << " km/h\n";
		cout << "Скорость:\t\t\t" << speed << " km/h\n";
	}

	Car(Tank tank, Engine engine, int max_speed = 250, int accelleration = 10) :
		tank(tank),
		engine(engine),
		speed(0),
		MAX_SPEED
		(
			max_speed< MAX_SPEED_MIN ? MAX_SPEED_MIN :
			max_speed>MAX_SPEED_MAX ? MAX_SPEED_MAX :
			max_speed),
		ACCELERATION
		(
			accelleration > MAX_ACCELERATION ? MAX_ACCELERATION :
			accelleration < MIN_ACCELERATION ? MIN_ACCELERATION :
			accelleration)
	{
		driver_inside = false;
	}
	~Car()
	{
	}

};

void main()
{
	setlocale(LC_ALL, "");


	Tank tank(50); Engine engine(40); Car car(tank, engine);

	cout << "Чтобы войти в машину нажмите Space\n" << "Чтобы заправить машину нажмите RCtrl" << endl;;
	car.machine_control();

}