# Do NOT change the ordering of these variables they have been thought out carefully and changing the order might break things

# Designate which compiler to use
CXX				= g++
# If for some reason you decide to use .cc files instead of cpp files update this extension
# Note that this Makefile will NOT work if you mix cpp and cc so stay consistent.
EXTENSION		= cc

# List of test cases (with main())
TESTSOURCES 	= $(wildcard test*.$(EXTENSION))
# List of multi CPU test cases (with main()) used for debugging only, DO NOT SUBMIT!
MULTISOURCES 	= $(wildcard multi_test*.$(EXTENSION))

# List of sources used in project
SOURCES 	 	= $(wildcard *.$(EXTENSION))
SOURCES        := $(filter-out $(TESTSOURCES), $(SOURCES))
SOURCES		   := $(filter-out $(MULTISOURCES), $(SOURCES)) 

# Uncomment and use these commands (replacing test_name) to ignore test cases that you **know** are failing but don't care about for now
# TESTSOURCES  := $(filter-out <test_name>.$(EXTENSION), $(TESTSOURCES))
# MULTISOURCES := $(filter-out <multi_test_name>.$(EXTENSION), $(MULTISOURCES))

# List of objects used in project
OBJECTS			= $(SOURCES:%.$(EXTENSION)=%.o)
MULTITESTOBJECTS = $(MULTISOURCES:%.$(EXTENSION)=%.o)
TESTOBJECTS     = $(TESTSOURCES:%.$(EXTENSION)=%.o)

# Names of test executables
TESTS       	= $(TESTSOURCES:%.$(EXTENSION)=%)
# Names of multi CPU test executables
MULTITESTS		= $(MULTISOURCES:%.$(EXTENSION)=%)

# The library we give you
LIBRARY			= libcpu.o

# Ideally we would figure out which files depend on which headers but its much easier to just depend on any headers changing
HEADERS 		= $(wildcard *.h)

# Default Flags
# Yes! -Werror is on, as it *should* be when you compile for best practices! If you want to cop out you can always delete it :(
CXXFLAGS = -std=c++11 -g3 -DASSERTENABLE
LDFLAGS  = $(LIBRARY) -ldl -pthread

# Text formatting. Set to empty to turn off formatting
bold		= \033[1m
normal		= \033[0m
green		= \033[0;32m

# Additional flags you may want to add to CXXFLAGS for debugging purposes: 
# -fstack-protector-all -Wstack-protector -fno-omit-frame-pointer 
# Look up what they do they may come in handy especially in the case of stack corruption

define make_tests
    $(1): compiling_output $(OBJECTS) $(HEADERS) $(1).o
	$(eval EXEC := $(strip $(1)))
	@$(CXX) $(CXXFLAGS) $(OBJECTS) $(EXEC).o $(LDFLAGS) -o $(EXEC)
	@echo "Running" $(EXEC)...
	@./$(EXEC) > $(EXEC).out
	@diff -q $(EXEC).out $(EXEC).correct
	@echo -e "$(green)Passed $(EXEC)$(normal)\n"
endef
$(foreach test, $(TESTS), $(eval $(call make_tests, $(test))))

define make_multi_tests
    $(1): compiling_output $(OBJECTS) $(HEADERS) $(1).o
	$(eval EXEC := $(strip $(1)))
	@$(CXX) $(CXXFLAGS) $(OBJECTS) $(EXEC).o $(LDFLAGS) -o $(EXEC)
	@echo "Running" $(EXEC)...
	@./$(EXEC) > $(EXEC).out
	@echo -e "$(green)Passed $(EXEC)$(normal)\n"
endef
$(foreach test, $(MULTITESTS), $(eval $(call make_multi_tests, $(test))))

# Target to run single CPU tests only
allsingletests: CXXFLAGS += -DDEBUG 
allsingletests: $(TESTS)

# Target to run multi CPU tests only
allmultitests: CXXFLAGS += -DDEBUG 
allmultitests: $(MULTITESTS)
# Target to run all your tests
alltests: CXXFLAGS += -DDEBUG 
alltests: $(TESTS) $(MULTITESTS)

# Target to compile basically anything
%.o: %.$(EXTENSION)
	@$(CXX) $(CXXFLAGS) -c $<

everything: thread* mutex* cv* cpu* libcpu.o
	g++  test-join-thorough2.cc thread* mutex* cv* cpu* $(CXXFLAGS) $(LDFLAGS) -o wat

define HELP_TEXT
###############################
$(bold)Makefile for EECS 482 Project 2$(normal)
###############################

$(bold)BEFORE READING THIS HELP$(normal)
	First, open up the Makefile and read the comments in there if you haven't already! 
	Not just to learn how to write a Makefile but also some of the comments have useful insights about the various facilities.

$(bold)REQUIREMENTS$(normal)
	Things you must do to let the Makefile do its magic:
		- Choose between using .cpp or .cc and set the EXTENSION variable in the Makefile accordingly.
		- All your files (test files included) must be in the same directory as the Makefile, or you can modify the Makefile to meet your needs.
		- Name your single CPU test cases with a test prefix just like the spec requires.
		- Name your multi CPU test cases which you will **NOT SUBMIT** with a multi_test prefix
		- Follow the rest of the rules listed below.

	The main goal of this Makefile is to automate building your thread library and linking to all your test cases and verifying correctness as much as possible.
	We want to make our computers do all the work!

$(bold)COMPILING$(normal)
	Your code will be compiled with these flags when you use the Makefile:
		-std=c++11 -Wall -Werror -Wextra -pedantic -g3 -DASSERTENABLE

	In addition the comments provide some additional flags that you may or may not find useful. 
	There are a couple of flags I want to point out:
	
		$(bold)-Werror$(normal)
		  Converts all warnings into errors (remove it if you want but really having this on is good practice) 
	
		$(bold)-g3$(normal) 
		  Maximum debug information (if your stack is corrupted even this flag won't be able to help you out :( )
		
		$(bold)-DASSERTENABLE$(normal) 
		  This defines ASSERTENABLE as a macro preprocessor which means you can use it to copiously assert in your code 
		  without fear of asserting when running on the autograder
		  How to use it?
			```
			#ifdef ASSERTENABLE
				#define <cassert>
				#define ASSERT(x) assert(x)
			#else
				#define ASSERT(x)
			#endif
			```
			Put this in a header file you include from everywhere then you can always just do ASSERT() to do an assertion and you're good to go
	
		$(bold)-DDEBUG$(normal)
		  Another flag that you should use is the DEBUG, for debug statements:
			```
			#ifdef DEBUG
				#define _(x) x 
			#else
				#define _(x) 
			#endif

			_(std::cerr << "Debug print" << std::endl);
			```
		  Now you can surround any statements you want with _(<code>) which will only appear when on debug mode. (Also note the use of cerr for debug prints)
		  This also lets you leave in your debug statements when you submit without fear of the autograder marking you down for it.

$(bold)RUNNING TEST CASES$(normal)
	Automatically running test cases is simple, the hard part is verifying that the test executed successfully. 
	With this Makefile you will use simple diff checking for checking correctness (although not always accurate it is a good sanity check). 
	When running with multiple CPUs things become non-deterministic so we cannot even check for correct output, 
	for this Makefile we just check that the program runs and finishes without any errors like a seg fault.
	You can invoke test cases using the following commands:
		$(bold)make <test_name>$(normal)
			Where test_name is the name of the test case you wrote with the extension. This will run a single test case
		$(bold)make allsingletests$(normal)
			This will run all test cases with single CPUs
		$(bold)make allmultitests$(normal)
			This will run all multi CPU test cases
		$(bold)make alltests$(normal)
			This is the most important and runs all single and multi CPU test cases

	Note that if nothing has changed in your library and test case will not get compiled again. You need to call make clean to 
	get rid of the executable and object files. The reason for doing this is efficiency, why waste time if nothing has really changed?

	Now the question is what are we comparing our output to for single threaded test cases? You need to create a .correct file called <test_name>.correct 
	which contains the correct output for the test case. It is a good idea to commit the .correct file to git so that you can notice when your outputs are changing.
	In addition, if you decide to change a test case you should update the .correct file together and make a single commit with both those changes, that way you 
	will find it easy to navigate your git history to hunt down regression bugs that did not exist before a particular change.

	Remember what I told you about using computers to automate your task? Well, you can do that to automatically create your correct files too! But that is an 
	exercise left up to you! (Use the autograder to validate that the correct is actually accurate, hint: the autograder informs you if you don't pass your own 
		test cases, can you use this to your advantage?)

$(bold)PRO TIP$(normal)
	Wouldn't it be awesome if everytime you tried to commit a new change to your repo it would automatically run your entire test suite before allowing 
	you to proceed with the commit? That way you know as soon as you are about to *create* a new bug that didn't exist before. To do that here's what 
	you need to do in a terminal:
	 $ cd <directory_with_git_repo>
	 $ cd .git/hooks
	 $ touch precommit
	 $ vi/emacs precommit
	Paste this in (without backticks):
```
#!/bin/sh

set -e
make clean
make alltests
make clean

```
	This will make it so that everytime you commit you check that you didn't break anything. Note that if you want your partners to do this,
	they must do this in their git directories manually too!
	Want to commit at 11:59 so you can submit and don't want to waste time running all the test cases? Use the --no-verify flag when you commit.

$(bold)FINAL NOTES$(normal)
	All the ugly commands are suppresed so that you don't see them on the console and you only see nice looking output. If you want to
	unsupress said commands, remove the @ that prefixes them. As a result some commands may take unusally long some times because they are busy compiling.
	Don't be afraid to change things in the Makefile, if you break something you will always have an original copy!

endef

export HELP_TEXT
# Help command
help: 
	@echo -e "$$HELP_TEXT"

compiling_output:
	@echo -e "Compiling...\n"

clean:
	@rm -rf $(OBJECTS) $(MULTITESTOBJECTS) $(TESTOBJECTS) $(TESTS) $(MULTITESTS) *.out
	@echo "Cleaned directory"

# These targets do not create any files mark as phony
.PHONY: all release debug clean alltests allmultitests help

# Disable built-in rules
.SUFFIXES:
