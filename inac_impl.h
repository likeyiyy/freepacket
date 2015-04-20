
#ifndef _INAC_IMPL_H_
#define _INAC_IMPL_H_

#include "rule_defs.h"
#include "if_stat.h"
#include "stream_stat.h"
#include "rule_stat.h"

const char *inac_get_error_desc(int32_t err);

char *inac_get_dev_name(int32_t dev_id);

int32_t inac_open(char *ifname, int32_t *dev_id);
void inac_close(int32_t dev_id);

void inac_set_master(int32_t dev_id, int32_t is_master);
int32_t inac_get_app_count(void);

int32_t inac_get_rx_streams(int32_t dev_id, int32_t * stream_count);
int32_t inac_get_tx_streams(int32_t dev_id, int32_t * stream_count);

void *inac_get_raw_pkt(int32_t dev_id, int32_t stream_id);
int32_t inac_get_pkt_port(int32_t dev_id, int32_t stream_id);
int32_t inac_get_raw_size(int32_t dev_id, int32_t stream_id);

void *inac_get_l2_pkt(int32_t dev_id, int32_t stream_id);
int32_t inac_get_l2_size(int32_t dev_id, int32_t stream_id);

void *inac_get_l3_pkt(int32_t dev_id, int32_t stream_id);
int32_t inac_get_l3_size(int32_t dev_id, int32_t stream_id);

void *inac_alloc_pkt(int32_t dev_id, int32_t stream_id);
int32_t inac_send_pkt(int32_t dev_id, int32_t stream_id, int32_t frame_len, int32_t port);

int32_t inac_add_l4_rule(int32_t dev_id, l4_rule_t *rules, int32_t count, int32_t *ret);
int32_t inac_del_l4_rule(int32_t dev_id, l4_rule_t *rules, int32_t count, int32_t *ret);
int32_t inac_del_l4_rule_id(int32_t dev_id, int32_t *rule_id, int32_t count, int32_t *ret);
int32_t inac_del_l4_rule_class(int32_t dev_id, int32_t *rule_class, int32_t count, int32_t *ret);

int32_t inac_get_l4_rule_classes(void);
int32_t inac_get_port_count(int32_t dev_id);

int32_t inac_open_port(int32_t dev_id, int32_t *ports, int32_t count);
int32_t inac_close_port(int32_t dev_id, int32_t *ports, int32_t count);

int32_t inac_enable_internal_loopback(int32_t dev_id, int32_t *ports, int32_t count);
int32_t inac_disable_internal_loopback(int32_t dev_id, int32_t *ports, int32_t count);

int32_t inac_enable_external_loopback(int32_t dev_id, int32_t *ports, int32_t count);
int32_t inac_disable_external_loopback(int32_t dev_id, int32_t *ports, int32_t count);

int32_t inac_get_l4_rule_stat(int32_t dev_id, int32_t *rule_classes, int32_t count, rule_stat_t *stats);
int32_t inac_clear_l4_rule_stat(int32_t dev_id, int32_t *rule_classes, int32_t count);

int32_t inac_get_port_stat(int32_t dev_id, int32_t *ports, int32_t count, if_stats_t *stats);
int32_t inac_clear_port_stat(int32_t dev_id, int32_t *ports, int32_t count);

int32_t inac_get_rx_stream_stat(int32_t dev_id, int32_t app_id, int32_t stream_id, stream_stat_t *stats);
int32_t inac_clear_rx_stream_stat(int32_t dev_id, int32_t app_id, int32_t stream_id);
	
int32_t inac_get_tx_stream_stat(int32_t dev_id, int32_t app_id, int32_t stream_id, stream_stat_t *stats);
int32_t inac_clear_tx_stream_stat(int32_t dev_id, int32_t app_id, int32_t stream_id);

int32_t inac_set_dist_policy(int32_t dev_id, int32_t policy);
int32_t inac_set_missed_action(int32_t dev_id, int32_t action);
int32_t inac_set_unknown_action(int32_t dev_id, int32_t action);
int32_t inac_set_log_para(int32_t dev_id, log_para_t *para);
int32_t inac_set_block_para(int32_t dev_id, block_para_t *para);
int32_t inac_set_rule_class_para(int32_t dev_id, rule_class_para_t *para);

int32_t inac_query_l4_rule_id(int32_t dev_id, int32_t *rule_id, int32_t rule_id_size, l4_rule_t *rules, int32_t *rules_size);
int32_t inac_query_l4_rule_class(int32_t dev_id, int32_t rule_class, int32_t rule_id, l4_rule_t *rules, int32_t *rules_size);
int32_t inac_check_l4_rule_id(int32_t dev_id, int32_t *rule_id, int32_t rule_id_size, int32_t *ret);
int32_t inac_check_l4_rule(int32_t dev_id, l4_rule_t *rules, int32_t rules_size, int32_t *ret);

int32_t inac_del_l4_rule_id_ex(int32_t dev_id, int32_t *rule_id, int32_t count, int32_t *ret, uint32_t ip_version);
int32_t inac_del_l4_rule_class_ex(int32_t dev_id, int32_t *rule_class, int32_t count, int32_t *ret, uint32_t ip_version);
int32_t inac_get_l4_rule_stat_ex(int32_t dev_id, int32_t *rule_classes, int32_t count, rule_stat_t *stats, uint32_t ip_version);
int32_t inac_clear_l4_rule_stat_ex(int32_t dev_id, int32_t *rule_classes, int32_t count, uint32_t ip_version);
int32_t inac_query_l4_rule_id_ex(int32_t dev_id, int32_t *rule_id, int32_t rule_id_size, l4_rule_t *rules, int32_t *rules_size, uint32_t ip_version);
int32_t inac_query_l4_rule_class_ex(int32_t dev_id, int32_t rule_class, int32_t rule_id, l4_rule_t *rules, int32_t *rules_size, uint32_t ip_version);
int32_t inac_check_l4_rule_id_ex(int32_t dev_id, int32_t *rule_id, int32_t rule_id_size, int32_t *ret, uint32_t ip_version);


int32_t inac_reboot(int32_t dev_id);

int32_t inac_boot_done(int32_t dev_id);
int32_t inac_set_image(int32_t dev_id, void *image, int32_t image_size);
int32_t inac_set_bootimage(int32_t dev_id, void *image, int32_t image_size);
int32_t inac_get_board_type(int32_t dev_id, int32_t *board_type);

int32_t inac_set_regex_graph(int32_t dev_id, uint32_t graph_type, void * graph_data, uint32_t graph_size, void * map_tbl, uint32_t map_tbl_size);


int32_t inac_pq_send_pkt(int32_t dev_id, int32_t stream_id, int32_t frame_len, int32_t port ,uint32_t write);
int32_t inac_get_anti_attack_stat(int32_t dev_id, anti_attack_stat_t *attack_stat);

#endif /* _INAC_IMPL_H_ */

