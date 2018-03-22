#!/usr/bin/env python

import pygtk
pygtk.require('2.0')
import gtk
import sys

def createHBox(url, name):
        box = gtk.HBox(True , 0)
        label = gtk.Label(name)
        button = gtk.LinkButton(url, "visit");

        box.pack_start(label, True, True, 0)
        box.pack_start(button, False, False, 0)

        label.show()
        button.show()
        box.show()
        return box



class Window:
    def __init__(self):
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.set_title("DidItChange")
        self.window.connect("delete_event", self.delete_event)
        self.window.connect("destroy", self.destroy);
        self.window.set_border_width(10)
        self.box = gtk.VBox(False, 0)
        self.window.add(self.box)

        self.sites = []
        for line in sys.stdin:
            if line == "":
                break
            else:
                name,url = line.split(",")
                self.sites.append(createHBox(url, name))

        if not self.sites:
            exit()
        for site in self.sites:
            self.box.pack_start(site, True, True, 0)

        self.box.show()
        self.window.show()
    def main(self):
        gtk.main()
    def push_button(self ,widget, data=None):
        print "Button %s has been pushed" % data
    def delete_event(self, widget, event, data=None):
        return False #destroy
    def destroy(self, widget, data=None):
        gtk.main_quit()

if __name__ == "__main__":
    window = Window()
    window.main()
