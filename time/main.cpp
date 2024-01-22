#include <iostream>
#include <time.h>

#include <SFML/Graphics.hpp>

constexpr unsigned int SizeLimit = 1000;

sf::RenderWindow window(sf::VideoMode(700, 400), "time", sf::Style::Resize | sf::Style::Close);
sf::View view;

sf::Font font;
sf::Text timeText{};

signed char lastSceond;
time_t nowTime;
tm* nowTimeStruct = nullptr;
char buffer[] = "00:00:00";

bool positionChange = false;
bool sizeChange = false;
sf::Vector2f lastButtonPosition;
sf::Clock clickClock;
//sf::RectangleShape box;

void event();
void event(sf::Event::KeyEvent& event, bool flag);
void event(sf::Event::MouseButtonEvent& event, bool flag);
void event(sf::Event::MouseMoveEvent& event);
void sleep();

void setTextSize(unsigned int size);
void setTextSize();
void setTextOrigin();

int main(int argc, char* argv[])
{
	font.loadFromFile("font.otf");
	timeText.setFont(font);
	timeText.setString(buffer);
	timeText.setFillColor(sf::Color::White);
	timeText.setPosition(0, -50);

	// box.setOutlineThickness(4);
	// box.setOutlineColor(sf::Color::Green);
	// box.setFillColor(sf::Color::Transparent);

	view.setCenter(0, 0);
	setTextSize();
	window.setView(view);

	while (window.isOpen())
	{
		event();
		time(&nowTime);
		nowTimeStruct = localtime(&nowTime);
		if (nowTimeStruct->tm_sec != lastSceond)
		{
			lastSceond = nowTimeStruct->tm_sec;
			sprintf(buffer, "%02d:%02d:%02d", nowTimeStruct->tm_hour, nowTimeStruct->tm_min, nowTimeStruct->tm_sec);
			timeText.setString(buffer);

			// {
			// 	auto b = timeText.getGlobalBounds();
			// 	box.setPosition(b.left, b.top);
			// 	box.setSize({ b.width,b.height });
			// }

			window.clear(sf::Color(sf::Color::Black));
			window.draw(timeText);
			//window.draw(box);
			window.display();
		}
		sleep();
	}

	return 0;
}

void event()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::KeyPressed:
			::event(event.key, true);
			break;
		case sf::Event::KeyReleased:
			::event(event.key, false);
			break;
		case sf::Event::Resized:
			view.setSize(event.size.width, event.size.height);
			window.setView(view);
			positionChange = false;
			sizeChange = false;
			lastSceond = 61;
			break;
		case sf::Event::MouseButtonPressed:
			::event(event.mouseButton, true);
			break;
		case sf::Event::MouseButtonReleased:
			::event(event.mouseButton, false);
			break;
		case sf::Event::MouseMoved:
			::event(event.mouseMove);
		default:
			break;
		}
	}
}

void event(sf::Event::KeyEvent& event, bool flag)
{
	using Key = sf::Keyboard;
	switch (event.code)
	{
	case Key::Escape:
		window.close();
		break;

	case Key::Space:
		if (flag)
		{
			timeText.setPosition(0, 0);
			positionChange = false;
			sizeChange = false;
			setTextSize();
			lastSceond = 61;
		}
		break;

	default:
		break;
	}
}

void event(sf::Event::MouseButtonEvent& event, bool flag)
{
	auto position = window.mapPixelToCoords({ event.x, event.y });

	if (clickClock.getElapsedTime() < sf::milliseconds(200) && flag)
	{
		//双击：恢复位置
		timeText.setPosition(0, 0);
		positionChange = false;
		sizeChange = false;
		setTextSize();
		lastSceond = 61;
	}
	else if (timeText.getGlobalBounds().contains(position) || sizeChange)
	{
		//改变大小
		sizeChange = flag;
		lastButtonPosition.x = position.x;
		lastButtonPosition.y = position.y;
	}
	else
	{
		//移动位置
		positionChange = flag;
		lastButtonPosition.x = position.x;
		lastButtonPosition.y = position.y;
	}

	clickClock.restart();
}

void event(sf::Event::MouseMoveEvent& event)
{
	if (positionChange)
	{
		auto position = window.mapPixelToCoords({ event.x, event.y });
		auto delta = position - lastButtonPosition;
		lastButtonPosition = position;
		timeText.move(delta);
		lastSceond = 61;
	}
	else if (sizeChange)
	{
		auto position = window.mapPixelToCoords({ event.x, event.y });
		unsigned size = (unsigned)(timeText.getCharacterSize() * timeText.getScale().x) + (unsigned)(position.x - lastButtonPosition.x);
		lastButtonPosition = position;
		setTextSize(size);
		lastSceond = 61;
	}
}

void sleep()
{
	sf::sleep(sf::milliseconds(10));
}

void setTextSize(unsigned int size)
{
	if (10 < size && size < SizeLimit)
	{
		timeText.setCharacterSize(size);
		timeText.setScale({ 1, 1 });
	}
	else if (size >= SizeLimit)
	{
		timeText.setCharacterSize(SizeLimit);
		float scale = (float)size / (float)SizeLimit;
		timeText.setScale({ scale, scale });
	}
	setTextOrigin();
}

void setTextSize()
{
	auto textBound = timeText.getGlobalBounds();
	auto windowSize = window.getSize();
	float radio = textBound.height / textBound.width;
	unsigned int size;

	size = windowSize.x * radio;
	if (size > windowSize.y) size = windowSize.y;
	textBound = timeText.getGlobalBounds();

	setTextSize(size);
}

void setTextOrigin()
{
	auto textBound = timeText.getLocalBounds(); //ignore Scale
	timeText.setOrigin(textBound.width / 2.0f, textBound.height / 2.0f);
}
