#include "Handler/LoggerHandler.hpp"
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <string>

LoggerHandler handler;

GtkWidget *window;
GtkWidget *FixedContainer;
GtkWidget *StartListeningButton;
GtkTextView *LoggerText;
GtkEntry *ipAddressEntry;
GtkEntry *portEntry;
GtkWidget *ClearButton;
GtkWidget *SaveButton;


GtkBuilder *Builder;


void CallBack(LogMessage *message);
int main(int argc, char *argv[])
{
    handler.SetCallBack(CallBack);
    gtk_init(&argc, &argv);
    Builder = gtk_builder_new_from_file("GladeFiles/LoggerApp.glade");
    window = GTK_WIDGET(gtk_builder_get_object(Builder, "mainwindow"));

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(Builder, NULL);

    FixedContainer = GTK_WIDGET(gtk_builder_get_object(Builder, "fixed1"));
    ClearButton = GTK_WIDGET(gtk_builder_get_object(Builder, "clearButton"));
    SaveButton = GTK_WIDGET(gtk_builder_get_object(Builder, "saveButton"));
    StartListeningButton = GTK_WIDGET(gtk_builder_get_object(Builder, "StartListeningButton"));
    LoggerText = GTK_TEXT_VIEW(gtk_builder_get_object(Builder, "loggerText"));
    ipAddressEntry = GTK_ENTRY(gtk_builder_get_object(Builder, "ipAddressEntry"));
    portEntry = GTK_ENTRY(gtk_builder_get_object(Builder, "portEntry"));
   
    gtk_text_view_set_editable(LoggerText, false);
    GtkTextBuffer *textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(LoggerText));
    handler.SetLoggerTextWidget(textBuffer);
    gtk_widget_show(window);
    gtk_main();

    return 0;
}

extern "C" void onClearButtonClicked(GtkButton *clearButton)
{
    handler.ClearScreen();
}

extern "C" void onSaveButtonClicked(GtkButton *saveButton)
{
    std::string fileName = "";
    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    fileName += std::to_string(1900 + ltm->tm_year);
    fileName += "_";
    fileName += std::to_string(1 + ltm->tm_mon);
    fileName += "_";
    fileName += std::to_string(ltm->tm_mday);
    fileName += "_";
    fileName += std::to_string(ltm->tm_hour);
    fileName += "_";
    fileName += std::to_string(ltm->tm_min);
    fileName += "_";
    fileName += std::to_string(ltm->tm_sec);
    fileName += ".csv";

    std::ofstream outFile(fileName.c_str());
    outFile << "Not Implemented :)\n";
    outFile.close();

}

extern "C" void onStartListeningButtonClicked(GtkButton *startListeningButton)
{
    static bool running = false;
    if (!running)
    {
        const char *ipAddress = gtk_entry_get_text(ipAddressEntry);
        const char *portStr = gtk_entry_get_text(portEntry);
        int32_t portInt = atoi(portStr);
        handler.StartListening(ipAddress, portInt);
        gtk_button_set_label(GTK_BUTTON(startListeningButton), "Stop Listening");
        running = true;
    }
    else
    {
        gtk_button_set_label(GTK_BUTTON(startListeningButton), "Start Listening");
        handler.StopListening();
        running = false;
    }
}

extern "C" void onDebugCheckBoxToggled (GtkToggleButton *debugCheckBox)
{
    handler.FilterChanged(EnumLogLevel::DEBUG);
}

extern "C" void onWarningCheckBoxToggled (GtkToggleButton *warningCheckBox)
{
    handler.FilterChanged(EnumLogLevel::WARNING);
}

extern "C" void onErrorCheckBoxToggled (GtkToggleButton *errorCheckBox)
{
    handler.FilterChanged(EnumLogLevel::ERROR);
}

extern "C" void onFatalCheckBoxToggled (GtkToggleButton *fatalCheckBox)
{
    handler.FilterChanged(EnumLogLevel::FATAL);
}

void CallBack(LogMessage *message)
{
    handler.OnMessageReceivedCallBack(message);
}
