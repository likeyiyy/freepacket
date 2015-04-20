
#ifndef _RULE_DEFS_H_
#define _RULE_DEFS_H_

#ifdef INAC_HOST_DRV
#ifdef INAC_LINUX
#include <linux/types.h>
#include <linux/kernel.h>
#endif
#ifdef INAC_WIN
#include "win_dep.h"
#endif

#else
#ifndef INAC_WIN
#include <stdint.h>
#endif
#ifdef INAC_WIN
#include "win_dep.h"
#endif
#ifdef INAC_CARD_SIDE
//#include <byteorder.h>
#endif

#endif

#include <asm/byteorder.h>

#define MAX_RULE_CAPACITY	1000000
#define MAX_IPV6_RULE_CAPACITY  10000

#define MAX_RULE_CLASS		16
#define MAX_RULE_PRIORITY	(MAX_RULE_CLASS)
#define RULE_DEFAULT_TTL    (90 * 1000)	/* in milliseconds */
#define RULE_DEFAULT_PRIORITY	3

#define RULE_INVALID_ID					0xffffffff

#define set_para_ip_version(p, v) ((p) = ((uint32_t)(p) |((uint32_t)(v) << 31)))
#define get_para_ip_version(p)  (((uint32_t)(p) & 0x80000000U) >> 31)
#define get_para_value(p) ((uint32_t)(p) & 0x7fffffffU)

#ifdef INAC_WIN
typedef struct
{
	uint32_t addr[4];
} gen_ip_addr_t;
#else
typedef __uint128_t   gen_ip_addr_t;
#endif

typedef enum dist_policy_e
{
	DIST_POLICY_NONE = 0,
	DIST_POLICY_SIP,
	DIST_POLICY_DIP,
	DIST_POLICY_SDIP,
	DIST_POLICY_SDIP_SDPORT,
	DIST_POLICY_MAX
} dist_policy_t;

typedef enum rule_error_e
{
	RULE_OK = 0,
	RULE_EXIST,
	RULE_NOT_EXIST,
	RULE_ID_EXIST,
	RULE_FULL,
	RULE_INTERNAL_ERROR,
} rule_error_t;

typedef enum rule_action_e
{
	ACTION_PASS = 0,
	ACTION_DROP,
	ACTION_SYNPASS,
	ACTION_MAX
} rule_action_t;

typedef enum rule_log_acion_e
{
	LOG_ACTION_NONE = 0,
	LOG_ACTION_SIMPLE,
	LOG_ACTION_COMPLEX,
	LOG_ACTION_MAX
} rule_log_action_t;

typedef enum rule_block_action_e
{
	BLOCK_ACTION_NONE = 0,
	BLOCK_ACTION_SYN_RST,
	BLOCK_ACTION_SYN_FIN,
	BLOCK_ACTION_RST,
	BLOCK_ACTION_FIN,
	BLOCK_ACTION_SYN_RST_ACK,
	BLOCK_ACTION_SYN_FIN_ACK,
	BLOCK_ACTION_RST_ACK,
	BLOCK_ACTION_FIN_ACK,
	BLOCK_ACTION_MAX
} rule_block_action_t;

typedef enum rule_type_s
{
	RULE_TYPE_DYNAMIC = 0,
	RULE_TYPE_STATIC,
	RULE_TYPE_MAX
} rule_type_t;

typedef struct l4_rule_s
{
	gen_ip_addr_t 	src_addr;
	gen_ip_addr_t 	dst_addr;
	uint16_t 	src_port;
	uint16_t 	dst_port;

	uint8_t  	protocol;
	uint8_t  	action;
	uint8_t  	log_action;
	uint8_t  	block_action;

	uint32_t 	cust_id;

	uint32_t	rule_id;

	uint8_t  	type;
	uint8_t  	rule_class;
	uint8_t		rule_version;  //ipv4 or ipv6
	uint8_t		reserved_1;
	uint32_t	reserved_2[3];/*  host����Ҫ����16�ֽڶ��� */

} l4_rule_t;

#if 0
typedef struct pag_full_rule_s
{
	gen_ip_addr_t sip;			 //Դip
	gen_ip_addr_t dip;		 //Ŀ��ip
	uint16_t sport;  		 //Դ�˿�
	uint16_t dport;		 //Ŀ�Ķ˿�

	uint32_t ruleid;		 //����id
	uint8_t optype;		 //�������ͣ����ӻ���ɾ������
	uint8_t ruletype; 		 //��������
	uint8_t iaction;		 //�Բ����ĵĴ�����(pass,drop,synpass)
	uint8_t send_action;	 //��������(���Ͳ�ͬ�ı��� )
	uint8_t log_action;		 //����־�����Ͳ�ͬ����־��
	uint8_t protocol; 		 //Э������
	uint16_t pad;		//����
}pag_full_rule_t;
#endif


typedef struct block_para_s
{
	uint32_t	dev_id;
	uint32_t	port;
	uint32_t	rand_max;
	uint32_t	rand_key;
	uint8_t		dst_mac[8];
	uint8_t		src_mac[8];
} block_para_t;


typedef enum rule_version_s
{
	RULE_VERSION_V4 = 0,
	RULE_VERSION_V6,
	RULE_VERSION_MAX
}rule_version_t;



#define MAX_LOG_SERVER	64

typedef struct log_para_s
{
	uint32_t	dev_id;
	uint32_t	port;		/* ����log���ĵĶ˿ں�*/
	uint32_t	server_count;	/* ����log���ĵķ���������*/
	uint32_t	server_ip[MAX_LOG_SERVER];		/* ����log���ĵķ�����IP */
	uint16_t	udp_dport;		/* ����log���ĵ�Ŀ��udp �˿ں�*/
	uint16_t	udp_sport;		/* ����log ���ĵ�Դudp�˿ں�*/
	uint32_t	src_ip;			/* ����log���ĵ�ԴIP��ַ*/
	uint8_t		src_mac[8];		/* ����log���ĵ�ԴMAC��ַ*/
	uint8_t		dst_mac[8]; 	/* log��������MAC��ַ*/
} log_para_t;

typedef struct rule_class_para_s
{
	int16_t		class_id;
	int16_t		priority;
	int32_t		ttl;
} rule_class_para_t;


#ifdef INAC_CARD_SIDE
static inline void block_para_ntoh(block_para_t *para)
{
	para->dev_id = custom_ntohl(para->dev_id);
	para->port = custom_ntohl(para->port);
	para->rand_max = custom_ntohl(para->rand_max);
	para->rand_key = custom_ntohl(para->rand_key);
}

static inline void log_para_ntoh(log_para_t *para)
{
	int32_t i;

	para->dev_id = custom_ntohl(para->dev_id);
	para->port = custom_ntohl(para->port);
	para->server_count = custom_ntohl(para->server_count);
	para->udp_dport = custom_ntohs(para->udp_dport);
	para->udp_sport = custom_ntohs(para->udp_sport);
	para->src_ip = custom_ntohl(para->src_ip);

	for (i = 0; i < MAX_LOG_SERVER; i ++)
		para->server_ip[i] = custom_ntohl(para->server_ip[i]);
}

static inline void rule_class_para_ntoh(rule_class_para_t *para)
{
	para->class_id = custom_ntohs(para->class_id);
	para->priority = custom_ntohs(para->priority);
	para->ttl = custom_ntohl(para->ttl);
}

#else /* INAC_CARD_SIDE */

#ifdef INAC_HOST_DRV

#ifdef INAC_LINUX
static inline void block_para_hton(block_para_t *para)
{
	para->dev_id = custom_cpu_to_be32(para->dev_id);
	para->port = custom_cpu_to_be32(para->port);
	para->rand_max = custom_cpu_to_be32(para->rand_max);
	para->rand_key = custom_cpu_to_be32(para->rand_key);
}

static inline void log_para_hton(log_para_t *para)
{
	int32_t i;

	para->dev_id = custom_cpu_to_be32(para->dev_id);
	para->port = custom_cpu_to_be32(para->port);
	para->server_count = custom_cpu_to_be32(para->server_count);
	para->udp_dport = custom_cpu_to_be16(para->udp_dport);
	para->udp_sport = custom_cpu_to_be16(para->udp_sport);
	para->src_ip = custom_cpu_to_be32(para->src_ip);

	for (i = 0; i < MAX_LOG_SERVER; i ++)
		para->server_ip[i] = custom_cpu_to_be32(para->server_ip[i]);
}

static inline void rule_class_para_hton(rule_class_para_t *para)
{
	para->class_id = custom_cpu_to_be16(para->class_id);
	para->priority = custom_cpu_to_be16(para->priority);
	para->ttl = custom_cpu_to_be32(para->ttl);
}


#else /* INAC_LINUX */

static inline void block_para_hton(block_para_t *para)
{
	para->dev_id = custom_cpu_to_be32(para->dev_id);
	para->port = custom_cpu_to_be32(para->port);
	para->rand_max = custom_cpu_to_be32(para->rand_max);
	para->rand_key = custom_cpu_to_be32(para->rand_key);
}

static inline void log_para_hton(log_para_t *para)
{
	int32_t i;

	para->dev_id = custom_cpu_to_be32(para->dev_id);
	para->port = custom_cpu_to_be32(para->port);
	para->server_count = custom_cpu_to_be32(para->server_count);
	para->udp_dport = (uint16_t)custom_cpu_to_be16(para->udp_dport);
	para->udp_sport = (uint16_t)custom_cpu_to_be16(para->udp_sport);
	para->src_ip = custom_cpu_to_be32(para->src_ip);

	for (i = 0; i < MAX_LOG_SERVER; i ++)
		para->server_ip[i] = custom_cpu_to_be32(para->server_ip[i]);
}
static inline void rule_class_para_hton(rule_class_para_t *para)
{
	para->class_id = (uint16_t)custom_cpu_to_be16(para->class_id);
	para->priority = (uint16_t)custom_cpu_to_be16(para->priority);
	para->ttl = custom_cpu_to_be32(para->ttl);
}
#endif /* INAC_LINUX */

#endif /* INAC_HOST_DRV */

#endif /* INAC_CARD_SIDE */


#define l4_rule_hton_ex(r) \
	({ \
		(r)->src_addr = hton128((r)->src_addr); \
		(r)->dst_addr = hton128((r)->dst_addr); \
		(r)->src_port = htons((r)->src_port); \
		(r)->dst_port = htons((r)->dst_port); \
		(r)->cust_id = htonl((r)->cust_id); \
		(r)->rule_id = htonl((r)->rule_id); \
	})



#define l4_rule_hton(r) \
	{ \
		(r)->src_addr = custom_htonl((r)->src_addr); \
		(r)->dst_addr = custom_htonl((r)->dst_addr); \
		(r)->src_port = custom_htons((r)->src_port); \
		(r)->dst_port = custom_htons((r)->dst_port); \
		(r)->cust_id = custom_htonl((r)->cust_id); \
		(r)->rule_id = custom_htonl((r)->rule_id); \
	}

#define l4_rule_ntoh(r) \
	{ \
		(r)->src_addr = custom_ntohl((r)->src_addr); \
		(r)->dst_addr = custom_ntohl((r)->dst_addr); \
		(r)->src_port = custom_ntohs((r)->src_port); \
		(r)->dst_port = custom_ntohs((r)->dst_port); \
		(r)->cust_id = custom_ntohl((r)->cust_id); \
		(r)->rule_id = custom_htonl((r)->rule_id); \
	}

//
// Add for l2 table
//
typedef struct {
    uint32_t list_free_head;
    uint32_t list_free_tail;

    uint32_t list_used_head;
    uint32_t list_used_tail;
}l2list_info_t;

typedef struct
{
    uint32_t server;   //���������
    uint32_t vm;       //�������ţ������⻯ȡ0xff
    uint32_t port;     //�˿ڱ��
    uint32_t app;      //Ӧ�ñ��
    uint32_t inet;     //1:��ͨ����mac   0:Ӧ��mac
    uint32_t stream;   //�󶨵���ID
	uint32_t stream_num;   //�󶨵�������

	uint32_t app_id_alloc; //��������е�app id

    uint64_t mac;
}vm_l2_rule_t;

typedef struct
{
    uint64_t mac;
}vm_l2rule_key_t;

typedef struct
{
    uint32_t server;   //���������
    uint32_t vm;       //�������ţ������⻯ȡ0xff
    uint32_t port;     //�˿ڱ��
    uint32_t app;      //Ӧ�ñ��
    uint32_t inet;     //1:��ͨ����mac   0:Ӧ��mac
    uint32_t stream;   //�󶨵���ID
	uint32_t stream_num;   //�󶨵�������

	uint32_t app_id_alloc; //��������е�app id

    uint32_t rule_id;    //�����_�Զ�����
    uint32_t hash_next;    //hash��ͻ

    uint32_t used_next;
    uint32_t used_prev;

}vm_l2rule_result_t;

typedef struct
{
    vm_l2rule_key_t key;
    vm_l2rule_result_t result;
}vm_l2rule_entry_t;

	#define vml2_rule_hton(r) \
	{ \
		(r)->server = custom_htonl((r)->server); \
		(r)->vm = custom_htonl((r)->vm); \
		(r)->port = custom_htonl((r)->port); \
		(r)->app = custom_htons((r)->app); \
		(r)->inet = custom_htons((r)->inet); \
		(r)->stream = custom_htonl((r)->stream); \
	}

#define vml2_rule_ntoh(r) \
	{ \
		(r)->server = custom_ntohl((r)->server); \
		(r)->vm = custom_ntohl((r)->vm); \
		(r)->port = custom_ntohl((r)->port); \
		(r)->app = custom_ntohl((r)->app); \
		(r)->inet = custom_ntohl((r)->inet); \
		(r)->stream = custom_ntohl((r)->stream); \
	}


#endif /* _RULE_DEFS_H_ */

