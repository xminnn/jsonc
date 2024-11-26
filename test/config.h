#pragma once

#ifndef int64
#define int64 long long
#endif

static const int k_tips_login_password_error = 1001; //  
static const int k_tips_login_failed = 1002; //  
static const int k_tips_login_server_closed = 1003; //  
static const int k_tips_login_server_full = 1004; //  
static const int k_tips_server_maintenance = 1005; //  
static const int k_tips_server_will_maintenance = 1006; //  
static const int k_tips_item_get = 2001; //  
static const int k_tips_item_not_enough = 2002; //  
static const int k_tips_item_use = 2003; //  
static const int k_tips_item_error = 2004; //  
static const int k_tips_item_full_to_email = 2005; //  
static const int k_tips_level_not_satisfy = 3001; //  
static const int k_tips_level_up = 3002; //  
static const int k_tips_level_exp_get = 3003; //  
static const int k_tips_mail_get_one = 4001; //  
static const int k_tips_type_notification = 1; // 通知
static const int k_tips_type_system = 2; // 系统
static const int k_tips_type_alter = 3; // 弹窗

// Tips
struct res_tips {
    int id; // ID
    int type[2]; // 提示类型
    int type_count_;
    int time; // 通知显示时间
    char template[128]; // 模版内容
};

// 主线
struct res_task_mainline {
    int id; // ID
    int condition;// 条件ID ref>condition
    int reward_id;// 奖励ID ref>reward
    char hide; // 是否隐藏(注意这个表不可删除，只可隐藏)
};

static const int k_global_params_daily_task_refresh = 1; //  
static const int k_global_params_week_task_refresh = 2; //  
static const int k_global_params_rename_cost = 101; //  
static const int k_global_params_reset_horoscope = 102; //  
// 全局参数
struct res_global_params {
    int id; // ID
    int param1[5]; // 整数参数
    int param1_count_;
    int64 param2[5]; // 时间参数
    int param2_count_;
};

// 功能开关
struct res_function {
    int id; // ID
    char name[128]; // 名称
    char desc[128]; // 描述
    int condition;// 条件ID ref>condition_type
};

static const int k_map_element_type_player = 1; // 玩家
static const int k_map_element_type_troop = 2; // 队列
static const int k_map_element_type_monster = 3; // 怪物
static const int k_map_element_type_resource = 4; // 资源地
static const int k_map_element_type_building = 5; // 建筑
static const int k_map_element_type_city = 6; // 城市
static const int k_map_element_type_kingbuilding = 7; // 王座
static const int k_map_element_type_max = 8; // Max

// 地图物
struct res_map_element {
    int id; // ID
    char name[128]; // 名称
    int type; // 类型
    int level; // 等级
    struct {
        int type;
        int num;
    } troop[5]; // 军队
    int troop_count_;
    int attr[10];// 属性 ref>attr
    int attr_count_;
    int attr_val[10]; // 属性值
    int attr_val_count_;
    int skills[4]; // 技能
    int skills_count_;
    int res[10];// 资源 ref>item
    int res_count_;
    int res_max[10]; // 资源最大容量(取单位前)
    int res_max_count_;
    int res_make[10]; // 资源每秒生产
    int res_make_count_;
    int res_unin[10]; // 资源获取单位
    int res_unin_count_;
    int res_rate[10]; // 资源生产概率(万分制)
    int res_rate_count_;
    struct {
        int times;
        int cooldown;
    } rebattle; // 重复击败控制
    struct {
        int min;
        int max;
    } num_for_block; // 每个大块数量
};

// 奖励
struct res_reward {
    int id; // ID
    char name[128]; // 名称
    int items[10];// 随机物品列表 ref>item
    int items_count_;
    struct {
        int min;
        int max;
    } nums[10]; // 随机物品数量(max=0表示固定取min)
    int nums_count_;
    int weights[10]; // 权重随机-权重
    int weights_count_;
    int weights_times; // 权重随机-次数
    char weights_repeatable; // 权重随机-重复获得
};

static const int k_attr_huangup_limit = 5; // 挂机时间上限(秒) 
static const int k_attr_health = 101; // 生命 
static const int k_attr_attack = 102; // 攻击 
static const int k_attr_defense = 103; // 防御 
static const int k_attr_attack_bonus = 104; // 攻击加成 
static const int k_attr_health_bonus = 105; // 生命加成 
static const int k_attr_defense_bonus = 106; // 防御加成 
static const int k_attr_block_rate = 107; // 格挡概率 
static const int k_attr_critical_rate = 108; // 暴击率 
static const int k_attr_critical_damage = 109; // 暴击伤害 
static const int k_attr_troop_move_speed = 110; // 军队移动速度 
static const int k_attr_troop_attack_speed = 111; // 军队攻击速度 
static const int k_attr_troop_jump = 201; // 军队超时空移动距离 
static const int k_attr_troop_jump_add = 202; // 军队超时空移动距离加成 
static const int k_attr_vtype_value = 0; // 值
static const int k_attr_vtype_per = 1; // 百分比

// 属性
struct res_attr {
    int id; // ID
    char name[128]; // 名称
    int max; // 最大值
    int vtype; // 值类型
    int vper_tar; // 百分比加成目标
};

static const int k_building_active_where_main = 1; // 主要
static const int k_building_active_where_second = 2; // 次要

// 建筑主效位
struct res_building_active {
    int id; // ID
    int where; // 所属
    int limit[5];// 限制类型(不填表示不限制) ref>building_type
    int limit_count_;
    int match_attr[10];// 匹配属性 ref>attr
    int match_attr_count_;
    int match_up[10]; // 匹配属性提升(千分比)
    int match_up_count_;
    char empty_icon[256]; // 空底纹
    char skill_effect; // 是否生效技能
};

// 建筑属性池
struct res_building_attr_pool {
    int id; // ID
    char name[128]; // 名称
    int atts[10];// 属性列表 ref>attr
    int atts_count_;
    int weigts[10]; // 随机权重
    int weigts_count_;
    struct {
        int min;
        int max;
    } init_range[10]; // 初始值范围
    int init_range_count_;
    struct {
        int min;
        int max;
    } up_range[10]; // 升级值范围
    int up_range_count_;
};

// 建筑建造券
struct res_building_certificate {
    int id; // ID
    char name[128]; // 名称
    int building_type;// 建筑类型 ref>building_type
    int quality; // 品质
    int main_attr_pool;// 主属性池 ref>building_attr_pool
    int main_level[5]; // 主属性出现等级
    int main_level_count_;
    int main_weight[5]; // 主属性升级权重
    int main_weight_count_;
    int second_attr_pool;// 次属性池 ref>building_attr_pool
    int second_level[5]; // 次属性出现等级
    int second_level_count_;
    int second_weight[5]; // 次属性升级权重
    int second_weight_count_;
    int max_level; // 最高等级
    int init_level[2]; // 初始化等级随机区间(最高等级*100/x函数作为权重)
    int init_level_count_;
};

// 建筑等级
struct res_building_level {
    int level; // 等级
    int quality; // 类型
    struct {
        int item;
        int num;
    } up_cost[10]; // 升级消耗(当前等级升到下一级)
    int up_cost_count_;
    struct {
        int item;
        int num;
    } destroy_get[10]; // 摧毁获得
    int destroy_get_count_;
};

// 建筑类型
struct res_building_type {
    int id; // ID
    char name[128]; // 名称
    int own_max; // 最大数量
    char empty_icon[256]; // 空白图标
    char can_destroy; // 是否可摧毁
    char can_recover; // 是否可恢复
};

// 成就
struct res_achivement {
    int id; // ID
    int condition;// 条件ID ref>condition
    int reward_id;// 奖励ID ref>reward
};

// 挂机
struct res_hungup {
    int id; // ID
    int attr;// 属性(绑定的属性基数) ref>attr
    int attr_add;// 加成属性 ref>attr
    int item_get;// 获得道具 ref>item
    int64 cycle; // 结算周期
};

// 星座
struct res_horoscope {
    int id; // ID
    char name[128]; // 名称
};

static const int k_horoscope_pool_type_perfect = 1; // 极好
static const int k_horoscope_pool_type_good = 2; // 好
static const int k_horoscope_pool_type_poor = 3; // 差

// 星座属性池
struct res_horoscope_pool {
    int id; // ID
    int type; // 类型
    int attrs[2];// 属性列表 ref>attr
    int attrs_count_;
    int vals[2]; // 属性值
    int vals_count_;
};

// 条件
struct res_condition {
    int id; // ID(条件类型*100000+x)
    char use[128]; // 通途
    char desc[128]; // 名称
    int condition_type;// 条件类型 ref>condition_type
    int condtion_params[5]; // 条件参数
    int condtion_params_count_;
    int condition_times; // 条件次数
};

static const int k_condition_type_level_acheve = 1; // 升等级到 
static const int k_condition_type_watch_add = 2; // 看广告 
static const int k_condition_type_getted_item = 3; // 获得道具 
static const int k_condition_type_have_item = 4; // 拥有道具 
static const int k_condition_type_condition_type_type_cumulation = 1; // 累积
static const int k_condition_type_condition_type_type_get = 2; // 获取

// 条件类型
struct res_condition_type {
    int id; // ID
    char name[128]; // 名称
    int condition_type_type; // 类型
};

// 每周任务
struct res_task_week {
    int id; // ID
    int condition;// 条件ID ref>condition
    int reward_id;// 奖励ID ref>reward
    char is_process; // 是否是进度奖励
};

// 每日任务
struct res_task_daily {
    int id; // ID
    int condition;// 条件ID ref>condition
    int reward_id;// 奖励ID ref>reward
    char is_process; // 是否是进度奖励
};

static const int k_user_init_type_item = 1; // 赠送道具
static const int k_user_init_type_building = 2; // 初始化建筑

// 玩家初始化
struct res_user_init {
    int id; // ID
    int type; // 类型
    int param[5]; // 参数
    int param_count_;
};

static const int k_alliance_param_request_expire = 1; //  
static const int k_alliance_param_request_max = 2; //  
static const int k_alliance_param_join_cooldown = 3; //  
static const int k_alliance_param_tech_rand_base_cost = 4; //  
// 联盟参数
struct res_alliance_param {
    int id; // ID
    int params[10]; // 参数
    int params_count_;
};

// 联盟科技
struct res_alliance_tech {
    int id; // ID
    char name[128]; // 名称
    int lv; // 品质等级
    int weight; // 随机权重
    int attr_id;// 加成属性 ref>attr
    struct {
        int min;
        int max;
    } attr_add; // 属性加成值
};

static const int k_alliance_lv_min = 1; //  
static const int k_alliance_lv_max = 8; //  
// 联盟等级
struct res_alliance_lv {
    int id; // ID
    int exp; // 升到下一级需要的经验
    int member_limit; // 成员人数限制
    int vice_limit; // 副会长人数限制
    int office_limit; // 官员人数限制
    int tech_max_level; // 个人科技随机最高品质
};

static const int k_role_level_min = 1; //  
static const int k_role_level_max = 24; //  
// 角色等级
struct res_role_level {
    int id; // 等级
    int exp; // 升下一级需要的exp
    int reward;// 升级奖励道具 ref>reward
    int up_mail; // 升级邮件
    int up_mail_reward;// 升级邮件奖励 ref>reward
    struct {
        int attr; //ref>attr
        int val;
    } lv_attr[10]; // 等级属性加成(累计)
    int lv_attr_count_;
};

static const int k_item_diamond = 1; // 钻石 
static const int k_item_exp = 2; // 经验 
static const int k_item_food = 3; // 粮食 
static const int k_item_stone = 4; // 矿石 
static const int k_item_wood = 5; // 木材 
static const int k_item_alliance_point = 11; // 联盟点数 
static const int k_item_task_daily_point = 12; // 每日任务点数 
static const int k_item_task_week_point = 13; // 每周任务点数 
static const int k_item_troop1 = 21; // 战斗机1 
static const int k_item_troop2 = 22; // 战斗机2 
static const int k_item_troop3 = 23; // 战斗机3 
static const int k_item_type_resource = 1; // 资源
static const int k_item_type_chest = 2; // 宝箱
static const int k_item_type_func_item = 3; // 功能道具
static const int k_item_func_reward = 1; // 奖励
static const int k_item_func_role_exp = 2; // 加经验
static const int k_item_func_building_core = 3; // 获得建造核心
static const int k_item_level_start1 = 1; // 绿色
static const int k_item_level_start2 = 2; // 蓝色
static const int k_item_level_start3 = 3; // 橙色
static const int k_item_level_start4 = 4; // 红色
static const int k_item_level_start5 = 5; // 紫色

// 道具
struct res_item {
    int id; // ID
    char name[128]; // 名称
    int type; // 类型
    int func; // 使用功能
    int func_params[5]; // 功能参数
    int func_params_count_;
    char auto_open; // 自动打开
    int level; // 级别
    int bag; // 是否进背包
    char icon[256]; // 图标
    int sellable; // 是否可以售卖
    int day_limit; // 每日获取上限
    int limit; // 拥有上限
    char desc[1024]; // 描述
    int get_channel; // 获取渠道
};

static const int k_item_background_item_bg_star1 = 1; //  
static const int k_item_background_item_bg_star2 = 2; //  
static const int k_item_background_item_bg_star3 = 3; //  
static const int k_item_background_item_bg_star4 = 4; //  
static const int k_item_background_item_bg_star5 = 5; //  
// 道具背景
struct res_item_background {
    int id; // ID
    char path[256]; // 图片路径
};

// 道具获取途径
struct res_item_get_way {
    int id; // ID
    char name[128]; // 名称
};

static const int k_mail_level_achive = 1; //  
static const int k_mail_join_alliance = 2; //  
static const int k_mail_item_full = 3; //  
static const int k_mail_type_system = 1; // 系统
static const int k_mail_type_battle = 2; // 战报
static const int k_mail_type_notice = 3; // 公告
static const int k_mail_type_alliance = 4; // 公会

// 邮件
struct res_mail {
    int id; // ID
    int type; // 类型
    char title[128]; // 标题
    char simple[128]; // 摘要/发送着
    char template[1024]; // 模板
    int64 expire_time; // 过期时间
};

// 锻造
struct res_forging {
    int id; // ID
    int target;// 锻造目标 ref>item
    int need_item[10];// 需要资源 ref>item
    int need_item_count_;
    int need_num[10]; // 需要资源数量
    int need_num_count_;
};

struct res_config {
    struct res_tips tips[1000];
    int tips_count_;
    struct res_task_mainline task_mainline[1000];
    int task_mainline_count_;
    struct res_global_params global_params[1000];
    int global_params_count_;
    struct res_function function[1000];
    int function_count_;
    struct res_map_element map_element[3000];
    int map_element_count_;
    struct res_reward reward[1000];
    int reward_count_;
    struct res_attr attr[100];
    int attr_count_;
    struct res_building_active building_active[100];
    int building_active_count_;
    struct res_building_attr_pool building_attr_pool[1000];
    int building_attr_pool_count_;
    struct res_building_certificate building_certificate[1000];
    int building_certificate_count_;
    struct res_building_level building_level[300];
    int building_level_count_;
    struct res_building_type building_type[100];
    int building_type_count_;
    struct res_achivement achivement[3000];
    int achivement_count_;
    struct res_hungup hungup[100];
    int hungup_count_;
    struct res_horoscope horoscope[100];
    int horoscope_count_;
    struct res_horoscope_pool horoscope_pool[100];
    int horoscope_pool_count_;
    struct res_condition condition[1000];
    int condition_count_;
    struct res_condition_type condition_type[100];
    int condition_type_count_;
    struct res_task_week task_week[100];
    int task_week_count_;
    struct res_task_daily task_daily[100];
    int task_daily_count_;
    struct res_user_init user_init[200];
    int user_init_count_;
    struct res_alliance_param alliance_param[100];
    int alliance_param_count_;
    struct res_alliance_tech alliance_tech[100];
    int alliance_tech_count_;
    struct res_alliance_lv alliance_lv[100];
    int alliance_lv_count_;
    struct res_role_level role_level[100];
    int role_level_count_;
    struct res_item item[1000];
    int item_count_;
    struct res_item_background item_background[100];
    int item_background_count_;
    struct res_item_get_way item_get_way[200];
    int item_get_way_count_;
    struct res_mail mail[1000];
    int mail_count_;
    struct res_forging forging[1000];
    int forging_count_;
};

