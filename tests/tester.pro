TEMPLATE = subdirs

DEFINES += LoggerManagement=0

#build the libraries and the tester executable. This depends upon file naming conventions.
SUBDIRS += \
    ../cppext \
    ../system \
    ../posix \
    ../tests
