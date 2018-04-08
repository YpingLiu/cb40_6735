#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <cust_gpio_usage.h>
#include <mach/mt_gpio.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_typedefs.h>
#include <mach/board.h>
#include <mach/irqs.h>
#include <mach/eint.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/timer.h>

#define TDWAKEUP   "tdwake"
#define WAKEPORT  GPIO3

static struct class *td_class;
static struct device *td_dev;
static struct cdev td_cdev;
static dev_t  td_devno;
static wait_queue_head_t td_wqueue;
static atomic_t triggers = ATOMIC_INIT(0);
static struct timer_list td_timer;

static int td_open(struct inode * fi, struct file *fl)
{
      printk("td_open.\n");
      return 0;
}

static int td_close(struct inode *fi, struct file *fl)
{
      return 0;
}
static ssize_t td_read(struct file *fi, char __user * buf, size_t len, loff_t *poff)
{
     printk("[tdwake] td_read\n");

     if(wait_event_interruptible(td_wqueue, (atomic_read(&triggers) > 0)))
     {
         printk("[tdwakeup] read failed.\n");
         return -EAGAIN;
     }

     copy_to_user(buf, "1", 1);

     atomic_set(&triggers, 0);
     printk("[tdwake] td_read exit.\n");
     return 1;
}
static unsigned int td_poll(struct file *fi, struct poll_table_struct *ptlb)
{
     unsigned int events = 0;

     poll_wait(fi, &td_wqueue, ptlb);

     if(atomic_read(&triggers) > 0)
          events |= POLLIN | POLLRDNORM;

     return events;
}
static struct file_operations f_tpops = {
	.owner    = THIS_MODULE,
        .open     = td_open,
	.release  = td_close,
        .read     = td_read,
        .poll     = td_poll,
};

static void td_wakeup_set_pin(int pin)
{
        mt_set_gpio_mode(pin, GPIO_MODE_00);
        mt_set_gpio_dir(pin, GPIO_DIR_IN);
        mt_set_gpio_pull_enable(pin, GPIO_PULL_DISABLE); 
}

void td_timer_irq(unsigned long data) 
{

      if(mt_get_gpio_in(WAKEPORT) == 0)
      {
          atomic_inc(&triggers);
          wake_up(&td_wqueue);
      }
}

static irqreturn_t td_wakeup_irq(int irq, void *data)
{
      printk("[tdwakeup] irq....\n");
     
      mod_timer(&td_timer, jiffies + msecs_to_jiffies(10));

      return IRQ_HANDLED;
}
static int __init tdwakeup_init(void)
{
     int ret = 0;

     ret = alloc_chrdev_region(&td_devno, 0, 1, TDWAKEUP);
     if(ret)
     {
          printk("[tdwakeup]alloc_chrdev_region failed.\n");
          goto err;
     }
     
     cdev_init(&td_cdev, &f_tpops);
     td_cdev.owner = THIS_MODULE;
     
     ret = cdev_add(&td_cdev, td_devno, 1);
     if(ret)
     {
          printk("[tdwakeup] cdev_add failed.\n");
          goto err1;
     }
     
     td_class = class_create(THIS_MODULE, TDWAKEUP);
     td_dev = device_create(td_class, NULL, td_devno, NULL, TDWAKEUP);

     init_waitqueue_head(&td_wqueue);
     
     td_wakeup_set_pin(WAKEPORT);

     init_timer(&td_timer);
     td_timer.function = td_timer_irq;

     ret = request_irq(mt_gpio_to_irq(WAKEPORT), td_wakeup_irq, IRQF_TRIGGER_FALLING, "td_wakeup_eint", NULL);
     if(ret)
     {
          printk("[tdwakeup] request_irq failed.\n");
          goto err2;
     }

     enable_irq_wake(mt_gpio_to_irq(WAKEPORT));


     return ret;
err2:
    cdev_del(&td_cdev);
err1:
     unregister_chrdev_region(td_devno, 1);
err:     
     return ret;
}

static void __exit tdwakeup_exit(void)
{
    del_timer(&td_timer);
    free_irq(mt_gpio_to_irq(WAKEPORT), NULL);
    device_del(td_dev);
    class_destroy(td_class);
    cdev_del(&td_cdev);
    unregister_chrdev_region(td_devno, 1);
}

module_init(tdwakeup_init);
module_exit(tdwakeup_exit);

MODULE_DESCRIPTION("Henry for t-road smart cars devices");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Henry shen<henryshen206@163.com>");
