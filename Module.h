/**
 * Module (mother class) : used to ensure each module is runable.
 * ---
 * Each derived class must define setup() and loop()
 */
 
class Module {
    public:
        Module *nextModule;
        Module();
        virtual void setup() {}
        virtual void loop() {}
        virtual String mainWebPage(String) {}
};

/**
 * Module list of whole application
 */
Module *listModules = NULL;

/**
 * Constructor, auto-adds itseft to the list
 */
Module::Module() {
    nextModule = listModules;
    listModules = this;
}
