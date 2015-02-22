/* Necessary includes for device drivers */
#include <linux/module.h> /* modulo de kernel */
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/types.h> /* size_t types */
#include <linux/proc_fs.h> /* /proc filesystem*/
#include <linux/miscdevice.h> /* /dev */
#include <linux/seq_file.h> /* sequence files */
#include <asm/uaccess.h> /* copy_from_user e copy_to_user */
#include <linux/delay.h> /* ssleep() */
#include <linux/kthread.h> /* thread */
#include "video.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Augusto Soares - Bruno Mazzi - Germano Leite - Lucas Caixeta");
MODULE_DESCRIPTION("Este módulo cria um device em /dev/device e uma entrada \
  em /proc/device. Torna disponivel uma lista de videos que podem ser acessados atravez da interface /dev/device \
  Recebe um inteiro na interface /dev/device e carrega o video referente da lista.");


extern struct video video1;
extern struct video video2;
extern struct video video3;
extern struct video video4;
extern struct video video5; 

struct video *videos[] = {
					&video1,
					&video2,
					&video3,
					&video4,
					&video5
				};



/* Global variables of the driver */

/* Major number */
int device_major = 60;

/* Buffer to store data */
char *device_buffer;

/* Pointer to video buffer */
char *video_buffer = NULL;

/* Indice do video no vetor de videos */
int video_index = 0;

/* Ponteiro para /proc */
struct proc_dir_entry *proc_file_entry;

/* Struct que detem a thread */
struct task_struct *task;

void* data;
int fl_video_end = 0;

int thread_function(void *data){

	while (!kthread_should_stop()){
		
		printk("Tocando video %d: %s\n",video_index, videos[video_index]->name);
		printk("Tamanho do video: %ld\n", videos[video_index]->len);
		ssleep(10);
		printk("Dormiu 10s\n");
	}
	return 0;
}

void imprimir(struct seq_file *m){
	
	pr_info("Imprimindo log\n");
	printk("Testando se vai imprimir no mesmo log");
	seq_printf(m, "Imprimindo na tela\n");
	seq_printf(m, "Imprimindo outra linha\n");
}

int device_open(struct inode *inode, struct file *filp) {

	pr_info("Abriu o device\n");
	/* Aloca memoria para o buffer de kernel */
    device_buffer = kzalloc(20, GFP_KERNEL); 

	/* Aponta o buffer para o video selecionado no indice */
	video_buffer = videos[video_index]->data;
	
	
    return 0;
}

/* Release do buffer */
int device_release(struct inode *inode, struct file *filp) {

	pr_info("Testando a funcao release\n");
  /* Libera memoria do buffer de kernel */
	kfree(device_buffer);


    return 0;
}

/* Leitura de dados do /dev nao permitida */
ssize_t device_read(struct file *filp, char *buf, size_t length, loff_t *f_pos) { 

    int bytes_read = 0;

	//pr_info("Now playing video %d - %s\n", video_index, videos[video_index]->name);
	
	if ((video_buffer - videos[video_index]->data) >= videos[video_index]->len){
			
			fl_video_end = 1;
	}

	while (length && (video_buffer - videos[video_index]->data) < videos[video_index]->len) {
		put_user(*(video_buffer++), buf++);

		length--;
		bytes_read++;
	}

	return bytes_read;
}

/*Escrita de dados do usuario no /dev/device */
ssize_t device_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
    
    pr_info("Escrevendo no /dev\n");
	printk("Testando se vai escrever na tela\n");
	/* Transfere dados do user para o buffer do kernel */
    copy_from_user(device_buffer, buf, count);
    
    /* Conversao da entrada para long */
    kstrtoint(device_buffer, 0, &video_index);

	return count;
//    pr_err("Writing to device isn't supported\n");
}

/* Estrutura relativa as operacoes de /dev */
struct file_operations dev_fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* Operações de leitura para /proc/primo */
int proc_read(struct seq_file *m, void *v){

  /* Calcula o primo */
  imprimir(m);

  return 0;
}

/* Operações de escrita para o /proc/device */
ssize_t proc_write(struct file *file, const char *buffer, size_t count, loff_t * off){
	
	pr_info("Escrevendo no proc\n");
	printk("Testando se vai escrever na tela\n");
	/* Transfere dados do user para o buffer do kernel */
    //copy_from_user(device_buffer, buffer, count);
    
    /* Conversao da entrada para long */
    //kstrtoint(device_buffer, 0, &video_index);

	
    return count;
}

/* Funcao de abertura de /proc */
int proc_open(struct inode *inode, struct file *file){
    
    pr_info("Open /proc \n"); 
	
    return single_open(file, proc_read, NULL);
}

/* Estrutura relativa a operacoes de /proc */
struct file_operations proc_file_fops = {
     .open  = proc_open,
     .read  = seq_read,
     .write = proc_write,
     .llseek  = seq_lseek
 };

/* Funcao init */
int device_init(void) {

    int deviceRegister = 0;
    
    /* Mensagem de insercao do modulo */
	printk("Inserting device module\n");

	/* Registro do /dev */
    deviceRegister = register_chrdev(device_major, "device", &dev_fops);
    if (deviceRegister < 0) {
        printk("memory: cannot obtain major number\n");
        return device_major;
    }

    /* Registro do /proc */
    proc_file_entry = proc_create("device", 0, NULL, &proc_file_fops);
    if(proc_file_entry == NULL){
      printk("FATAL ERROR ON PROC REGISTER - SHUTTING DOWN");
    }
    
    task = kthread_run(&thread_function,(void *)data,"device_thread");

    return 0;
}

/* Funcao exit */
void device_exit(void) {

  /* Remocao de dispositivo /dev */
    unregister_chrdev(device_major, "device");

    /* Remocao de entrada /proc */
    proc_remove(proc_file_entry);
    
    kthread_stop(task);

    /* Mensagem de remocao do modulo */
    printk("Removing device module\n");
}


/* Declaration of the init and exit functions */
module_init(device_init);
module_exit(device_exit);
