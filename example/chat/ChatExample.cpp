#include "../../include/wwidget/Window.hpp"
#include "../../include/wwidget/widget/List.hpp"
#include "../../include/wwidget/widget/Label.hpp"
#include "../../include/wwidget/widget/TextField.hpp"

#include "Chat.hpp"

using namespace wwidget;

class ChatWindow {
	Chat mChat;

	Window     mWindow;
	List*      mQuestionPane;
	List*      mRootPane;
	List*      mChatHistory;
	TextField* mInput;

	std::string mName;

	void setDefaultInputCallback() {
		mInput->onReturn([this]() {
			mChat.send(mInput->content());
			mInput->content("");
		});
	}

	/*
	void askForPort(std::function<void()> then = nullptr) {
		Label* msg = mRootPane->add<Label>()->content("Enter a port:");
		mInput->content("4442");
		mInput->onUpdateCallback = [=](TextField* tf) {
			mChat.listen(std::stoul(tf->content(), nullptr, 10));
			msg->remove();
			tf->content("");
			if(then)
				then();
		};
	}
	*/
public:
	ChatWindow() :
		mWindow(
			"Chat client at port 4442", 800, 600,
			// Window::FlagShrinkFit |
			Window::FlagDoublebuffered |
			Window::FlagVsync
		)
	{
		mRootPane = mWindow.add<List>();
		mRootPane->flow(List::FlowUp);
		mRootPane->align(Widget::AlignFill);

		mInput = mRootPane->add<TextField>();
		mInput->align(Widget::AlignFill);
		setDefaultInputCallback();

		mChatHistory = mRootPane->add<List>();

		mQuestionPane = mRootPane->add<List>();
	}

	~ChatWindow() {}

	void run() {
		// Show username option
		{
			List* uname = mWindow.add<List>()->flow(List::FlowRight);
			uname->add<Label>()->content("Username:")->align(Widget::AlignMin);
			uname->add<TextField>()
				->content(mChat.username())
				->onReturn([this]() {
					mChat.username(mInput->content());
				})
				->align(Widget::AlignMax, Widget::AlignMin);
		}

		mChat.onReceive([this](const char* uname, const char* msg) {
			addMessage(std::string("[") + uname + "] " + msg);
		});
		mChat.listen();

		mWindow.keepOpen();
	}

	void send(std::string const& s) {
		addMessage("[You] " + s);
	}

	void addMessage(std::string const& s) {
		mChatHistory->add<Label>()->content(s);
	}
};

int main(int argc, char const** argv) {
	ChatWindow client;
	client.run();
	return 0;
}
