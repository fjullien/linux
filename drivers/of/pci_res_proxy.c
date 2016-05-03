/*
 * Copyright (C) 2016 Franck Jullien <franck.jullien@odyssee-systemes.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/pci.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>

struct pci_proxy_priv {
	struct irq_domain *domain;
	struct pci_dev *pci_dev;
};

static const struct irq_domain_ops pci_proxy_irq_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
};

static int pci_proxy_update_range_property(struct device_node *np, struct pci_dev *pci_dev)
{
	struct property *newprop;
	struct property *oldprop;
	u32 *addr;
	int ret;
	int bars;
	int i;

	oldprop = of_find_property(np, "ranges", NULL);

	newprop = kzalloc(sizeof(*newprop) + oldprop->length, GFP_KERNEL);
	if (newprop == NULL)
		return -1;

	newprop->name = "ranges";
	newprop->length = oldprop->length;
	newprop->value = newprop + 1;
	addr = newprop->value;

	/* A range line is composed of 4 elecments */
	bars = of_property_count_u32_elems(np, "ranges") / 4;

	for (i = 0; i < bars; i++) {
		addr[i]     = __cpu_to_be32(i);
		addr[i + 1] = 0;
		addr[i + 2] = __cpu_to_be32(pci_resource_start(pci_dev, i));
		addr[i + 3] = __cpu_to_be32(pci_resource_len(pci_dev, i));
	}

	ret = of_update_property(np, newprop);

	return 0;
}

static int pci_proxy_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct pci_proxy_priv *priv;

	priv = kzalloc(sizeof(struct pci_proxy_priv), GFP_KERNEL);
	if (priv == NULL) {
		pr_err("%s: Couldn't allocate device private record\n",
		       dev_name(&pdev->dev));
		return -ENOMEM;
	}

	priv->pci_dev = np->data;
	dev_set_drvdata(&pdev->dev, (void *)priv);

	priv->domain = irq_domain_add_simple(np, 1, priv->pci_dev->irq,
					     &pci_proxy_irq_domain_ops, NULL);

	pci_proxy_update_range_property(np, priv->pci_dev);

	return 0;
}

static int pci_proxy_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id pci_proxy_of_match[] = {
	{ .compatible = "generic,pci-proxy", },
	{ /* end of list */ },
};

MODULE_DEVICE_TABLE(of, pci_proxy_of_match);

static struct platform_driver pci_proxy_driver = {
	.probe		= pci_proxy_probe,
	.remove		= pci_proxy_remove,
	.driver		= {
			.name = "pci_proxy",
			.of_match_table	= pci_proxy_of_match,
	},
};

static int __init pci_proxy_init(void)
{
	return platform_driver_register(&pci_proxy_driver);
}

subsys_initcall(pci_proxy_init);

static void __exit pci_proxy_exit(void)
{
	platform_driver_unregister(&pci_proxy_driver);
}
module_exit(pci_proxy_exit);

MODULE_AUTHOR("Franck Jullien");
MODULE_DESCRIPTION("PCI ressources proxy");
MODULE_LICENSE("GPL");
