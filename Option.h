/**
 * Option (mother class) : used to ensure each option is up.
 * ---
 * Each derived class must define setup() and loop()
 */
 
class Option {
    public:
        Option *nextOption;
        Option();
        virtual void setup() {}
        virtual void loop() {}
};

/**
 * Module list of whole application
 */
Option *listOptions = NULL;

/**
 * Constructor, auto-adds itseft to the list
 */
Option::Option() {
    nextOption = listOptions;
    listOptions = this;
}

