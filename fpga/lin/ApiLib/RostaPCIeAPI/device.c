
#include "RostaPCIeAPI.h"

static const char *devices_path = "/sys/bus/pci/drivers/" DRIVER_NAME;


#define TMPLEN 256
static char temp[TMPLEN];

// INTERNAL REGION ROUTINES

static int _region_init(struct pci_region *r)
{
	r->base = 0;
	r->length = 0;

	r->fd = 0;
	
	r->localbase = NULL;
	r->localoffset = 0;
	
	return 0;
}

static int _region_close(struct pci_region *r)
{		
	if (r->localbase != NULL)
	{
		munmap(r->localbase, r->length+r->localoffset);
	}
	
	if (r->fd > 0)
	{
		close(r->fd);
	}
	
	//_region_init(r);
	
	return 0;
}

// EXTERNALS

int pcid_device_open(struct pci_device *p)
{
	struct stat buffer;
	int r;
	int i;
	FILE *fp;
	unsigned int start, length, flags;
	
	_region_init(&(p->config));
		
	sprintf(temp,"%s/%04x:%02x:%02x.%d/config", devices_path, 
			p->phys_domain, p->phys_bus, p->phys_device, p->phys_function
	);
		
	r = stat(temp, &buffer);
	
	if (r == 0)
	{
		p->config.length = buffer.st_size;
	}
	
	sprintf(temp,"%s/%04x:%02x:%02x.%d/resource", devices_path, 
			p->phys_domain, p->phys_bus, p->phys_device, p->phys_function
	);
	
	fp = fopen(temp, "r");
	
	if (fp == NULL)
	{
		return -2;
	}
	
	for (i=0; i<6; i++)
	{
		_region_init(&(p->regions[i]));
		
		r = fscanf(fp,"%x %x %x\n",&start, &length, &flags);
		
		if (r != 3)
		{
			return -3;
		}
		
		if ((start == 0) && (length == 0)) continue;
		 
		length = 1 + (length - start);
				
		p->regions[i].base   = start;
		p->regions[i].length = length;
		
	}

	return 0;
}

int pcid_device_close(struct pci_device *p)
{
	int i;

	_region_close(&(p->config));
	
	for (i=0; i<6; i++)
	{
		_region_close(&(p->regions[i]));
	}

	return 0;
}

int RD_PrintDeviceInfo(struct pci_device *p)
{
	int i;
	char *t = temp;
	
	t += sprintf(t, "%20s: 0x%04x\n","Vendor", p->vendor_id);
	t += sprintf(t, "%20s: 0x%04x\n","Device", p->device_id);
	t += sprintf(t, "%20s: 0x%04x\n","Subsystem Vendor", p->subsys_vendor_id);
	t += sprintf(t, "%20s: 0x%04x\n","Subsystem Device", p->subsys_device_id);
	t += sprintf(t, "%20s: %04x:%02x:%02x.%d\n","Location", p->phys_domain, p->phys_bus, p->phys_device, p->phys_function);
	t += sprintf(t, "%20s: %04x:%02x:%02x.%d\n","Parent", p->parent[0].parent_phys_domain, p->parent[0].parent_phys_bus, p->parent[0].parent_phys_device, p->parent[0].parent_phys_function);
	
	t += sprintf(t, "%20s: %d\n","Config", p->config.length);
	
	for (i=0; i<6; i++)
	{
		t += sprintf(t, "%20s: %d base: 0x%8x length:%d\n","Resource", i, p->regions[i].base,  p->regions[i].length);
	}
	
	puts(temp);
	
	return 0;
}

// MORE INTERNALS

static int _pci_region_open(struct pci_device *p, int region)
{
	struct pci_region *r;
	int fd;
	void *base;
	
	unsigned length, start, pagesize, offset;
	
	r = &(p->regions[region]);
		
	fd = open("/dev/mem",O_RDWR | O_SYNC);
	
	if (fd < 0)
	{
		return -10;
	}
	
	length = r->length;
	start = r->base;
	
	pagesize = getpagesize();
	
	offset = start & (pagesize-1);
		
	length = length + offset;
	start = start - offset;
	
	base = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, start);
	
	if (base == (void *)-1)
	{
		close(fd);
		return -11;
	}
	
	r->localbase = base;
	r->localoffset = offset;
	r->fd = fd;
	
	return 0;
}

static int _pci_config_open(struct pci_device *p)
{
	struct pci_region *r;
	int fd;
	
	r = &(p->config);
	
	sprintf(temp,"%s/%04x:%02x:%02x.%d/config", devices_path, 
			p->phys_domain, p->phys_bus, p->phys_device, p->phys_function
	);
	
	fd = open(temp,O_RDWR);
	
	if (fd < 0)
	{
		return -1;
	}
	
	r->fd = fd;
	
	return 0;
}

// MORE EXTERNALS

int pcid_config_read(struct pci_device *p, unsigned size, unsigned offset, unsigned *result)
{
	int r;
	unsigned int t32;
	unsigned short t16;
	unsigned char t8;
	
	if ((offset+size) > p->config.length)
	{
		return -1;
	}
	
	if (p->config.fd == 0)
	{
		r = _pci_config_open(p);
		
		if (r != 0)
		{
			return r;
		}
	}
	
	if (size == 1)
	{
		r = pread(p->config.fd, (void *)&t8, 1, offset);
		if (r != 1)
		{
			return -2;
		}
		*result = t8;
		return 0;
	}
	else if (size == 2)
	{
		r = pread(p->config.fd, (void *)&t16, 2, offset);
		if (r != 2)
		{
			return -2;
		}
		*result = t16;
		return 0;
	}
	else if (size == 4)
	{
		r = pread(p->config.fd, (void *)&t32, 4, offset);
		if (r != 4)
		{
			return -2;
		}
		*result = t32;
		return 0;
	}
	
	return -3;
}

int pcid_config_dump(struct pci_device *p, int size)
{
	int r;
	unsigned data;
	unsigned perline = 0;
	unsigned address;
	char *format;
	
	if (size == 1) format = " %02x";
	else if (size == 2) format = " %04x";
	else if (size == 4) format = " %08x";
	else return -1;
	
	for (address = 0; address < p->config.length; address += size)
	{
		if (perline == 0)
		{
			printf("\n%04x:", address);
			perline = 16 / size;
		}
		
		r = pcid_config_read(p,size,address,&data);
		
		if (r < 0) return r;
		
		printf(format,data);
		
		perline --;
	}
	
	printf("\n");
	
	return 0;
}

int pcid_region_read(struct pci_device *p, unsigned region, unsigned size, unsigned offset, unsigned *result)
{
	int r;
	void *ptr;
		
	if (region > 5)
	{
		return -1;
	}
	
	if ((offset+size) > p->regions[region].length)
	{
		return -1;
	}
	
	if (p->regions[region].localbase == NULL)
	{
		r = _pci_region_open(p,region);
		
		if (r != 0)
		{
			return r;
		}
	}
	
	ptr = (void *)(p->regions[region].localbase + p->regions[region].localoffset + offset);
	
	if (size == 1)
	{
		*result = *((unsigned char *)ptr);
		return 0;
	}
	else if (size == 2)
	{
		*result = *((unsigned short *)ptr);
		return 0;
	}
	else if (size == 4)
	{
		*result = *((unsigned int *)ptr);
		return 0;
	}
	
	return -3;
}

int pcid_region_write(struct pci_device *p, unsigned region, unsigned size, unsigned offset, unsigned data)
{
	int r;
	void *ptr;
	
	if (region > 5)
	{
		return -1;
	}
	
	if ((offset+size) > p->regions[region].length)
	{
		return -1;
	}
	
	if (p->regions[region].fd == 0)
	{
		r = _pci_region_open(p,region);
		
		if (r != 0)
		{
			return r;
		}
	}

	ptr = (void *)(p->regions[region].localbase + p->regions[region].localoffset + offset);
	
	if (size == 1)
	{
		*((unsigned char *)ptr) = data;
		return 0;
	}
	else if (size == 2)
	{
		*((unsigned short *)ptr) = data;
		return 0;
	}
	else if (size == 4)
	{
		*((unsigned int *)ptr) = data;
		return 0;
	}
	
	return -3;
}		
		
int pcid_region_dump(struct pci_device *p, int region, int size)
{
	int r;
	unsigned data;
	unsigned perline = 0;
	unsigned address;
	char *format;
	
	if (size == 1) format = " %02x";
	else if (size == 2) format = " %04x";
	else if (size == 4) format = " %08x";
	else return -1;
	
	for (address = 0; address < p->regions[region].length; address += size)
	{
		if (perline == 0)
		{
			printf("\n%04x:", address);
			perline = 16 / size;
		}
		
		r = pcid_region_read(p,region,size,address,&data);
		
		if (r < 0)
		{
			printf("Error from region_read\n");
			return r;
		}
		
		printf(format,data);
		
		perline --;
	}
	
	printf("\n");
	
	return 0;
}

	
