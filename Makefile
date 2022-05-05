KERN_DIR =  /home/kernel-4.18.0-193.6.3.el8_2 
all:
	make -C $(KERN_DIR) M=`pwd` modules   
clean:   
	make -C $(KERN_DIR) M=`pwd` modules clean   
obj-m += myModule.o
