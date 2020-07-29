#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/time.h>

#define BUF_SIZE 200

// Duplicated in opersyshw.c
#define CMD_BASE                  0x12341
#define CMD_ZERO_CONTENT          CMD_BASE + 0
#define CMD_IS_THERE_CONTENT      CMD_BASE + 1
#define CMD_WRITE_TS              CMD_BASE + 2
#define CMD_GET_READN             CMD_BASE + 3
#define CMD_GET_WRITEN            CMD_BASE + 4
#define CMD_SET_BUFFER_CHAR       CMD_BASE + 5


static char buf[BUF_SIZE];
static char *read_ptr;
static char *write_ptr;
static unsigned int read_count = 0;
static unsigned int write_count = 0;
static struct timespec last_write;

static int device_open(struct inode *inode, struct file *file)
{
  printk("device_open called \n");

  return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
  printk("device_release called \n");

  return 0;
}

static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{
  int chars_read = 0;

  printk("device_read called \n");
  read_count++;

  while(length && *read_ptr && (read_ptr != write_ptr)) {
    put_user(*(read_ptr++), buffer++);

    printk("Reading %c \n", *read_ptr);

    if(read_ptr >= buf + BUF_SIZE)
      read_ptr = buf;

    chars_read++;
    length--;
  }

  return chars_read;
}

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
  int i;

  printk("device_write called \n");
  write_count++;
  last_write = CURRENT_TIME;

  for(i = 0; i < len; i++) {
    get_user(*write_ptr, buff++);
    printk("Writing %c \n", *write_ptr);
    write_ptr++;
    if (write_ptr >= buf + BUF_SIZE)
      write_ptr = buf;
  }

  return len;
}

static ssize_t
device_fill(struct file *filp, const char *ch)
{
  int i = 0;

  printk("device_fill called \n");

  printk("Writing %d x %c \n", BUF_SIZE, *ch);

  write_ptr = buf;
  for (i = 0; i < BUF_SIZE; i++)
  {
      get_user(*write_ptr, ch);
      write_ptr++;
  }

  return BUF_SIZE;
}

static ssize_t
device_ioctl(
    struct file *file,
    unsigned int cmd,
    unsigned long arg
)
{
  char *tmp;

  switch (cmd)
  {
    case CMD_ZERO_CONTENT:
      read_ptr = write_ptr;
      break;
    case CMD_IS_THERE_CONTENT:
      return (read_ptr != write_ptr);
      break;
    case CMD_WRITE_TS:
      return last_write.tv_sec;
    case CMD_GET_READN:
      return (ssize_t) read_count;
    case CMD_GET_WRITEN:
      return (ssize_t) write_count;
    case CMD_SET_BUFFER_CHAR:
      /* Get the parameter given to ioctl by the process */
      tmp = (char *)ioctl_param;
      device_fill(file, tmp);

      break;
    default:
      break;
  }

  return 0;
}

static struct file_operations fops = {
  .open           = device_open,
  .release        = device_release,
  .read           = device_read,
  .write          = device_write,
  .unlocked_ioctl = device_ioctl,
};

static struct miscdevice circ_char_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "circchar",
  .fops = &fops,
};

int circ_char_enter(void)
{
  int retval;

  retval = misc_register(&circ_char_misc);
  printk("CIRC Driver got retval %d\n", retval);
  printk("mmap is %08X\n", (int) fops.mmap);

  read_ptr = buf;
  write_ptr = buf;

  return 0;
}

void circ_char_exit(void)
{
  misc_deregister(&circ_char_misc);
}

module_init(circ_char_enter);
module_exit(circ_char_exit);

// vim: set ts=2 sw=2 tw=0 et :
