<template>
  <v-container>
    <h1>配送管理</h1>
    
    <!-- 加载指示器 -->
    <v-progress-circular v-if="loading" indeterminate color="primary"></v-progress-circular>
    
    <!-- 显示配送中的订单 -->
    <v-data-table v-if="!loading && deliveringOrders.length > 0" :headers="tableHeaders" :items="deliveringOrders">
      <template v-slot:item.actions="{ item }">
        <v-btn color="green" @click="confirmDelivered(item)">确认已送达</v-btn>
      </template>
    </v-data-table>
    <v-alert v-else-if="!loading && deliveringOrders.length === 0" type="info" dense>没有配送中的订单。</v-alert>

    <!-- 显示准备配送的订单 -->
    <v-data-table v-if="!loading && predeliveringOrders.length > 0" :headers="tableHeaders" :items="predeliveringOrders">
      <template v-slot:item.actions="{ item }">
        <v-btn color="blue" @click="confirmDelivery(item)">确认配送</v-btn>
      </template>
    </v-data-table>
    <v-alert v-else-if="!loading && predeliveringOrders.length === 0" type="info" dense>没有准备配送的订单。</v-alert>

  </v-container>
</template>

<script>
import { mapGetters, mapActions } from 'vuex';

export default {
  name: 'Deliveries',
  data() {
    return {
      tableHeaders: [
        { text: '订单编号', value: 'order_id', align: 'center' },
        { text: '客户姓名', value: 'customer_name' },
        { text: '客户电话', value: 'customer_phone' },
        { text: '配送地址', value: 'delivery_address' },
        { text: '菜品信息', value: 'order_items' },
        { text: '总价', value: 'total_price' },
        { text: '状态', value: 'status' },
        { text: '下单时间', value: 'order_time' },
        { text: '操作', value: 'actions', sortable: false },
      ],
      loading: false, // 添加加载状态
    };
  },
  computed: {
    ...mapGetters('orders', ['allOrders']),
    ...mapGetters('auth', ['userId']), // 获取当前登录用户的 ID

    // 获取准备配送的订单
    predeliveringOrders() {
      // 请根据后端实际的订单状态名称调整这里的过滤条件
      const orders = this.allOrders.filter(order => order.status === 'predelivering'); // 假设状态为 'preparing'
      console.log('准备配送的订单:', orders);
      return orders;
    },

    // 获取配送中的订单
    deliveringOrders() {
      const orders = this.allOrders.filter(order => 
        order.status === 'delivering' && 
        String(order.delivery_person?.delivery_id) === String(this.userId) // 使用 delivery_person.delivery_id
      );
      console.log('配送中的订单:', orders);
      return orders;
    },
  },
  methods: {
    ...mapActions('orders', ['fetchAllOrders', 'updateOrderStatus']),

    // 确认配送
    async confirmDelivery(order) {
      try {
        await this.updateOrderStatus({ orderId: order.order_id, status: 'delivering' });
        await this.updateDeliveryPerson(order.order_id);  // 更新配送员信息
        this.$emit('snackbar', { message: `订单 #${order.order_id} 已确认配送`, color: 'success' });
      } catch (error) {
        console.error('确认配送失败', error);
        this.$emit('snackbar', { message: '确认配送失败，请稍后再试', color: 'error' });
      }
    },

    // 更新配送员信息
    async updateDeliveryPerson(orderId) {
      try {
        await this.$store.dispatch('orders/updateOrderDeliveryPerson', { orderId, deliveryPersonId: this.userId });
      } catch (error) {
        console.error('更新配送员失败', error);
        throw error;
      }
    },

    // 确认已送达
    async confirmDelivered(order) {
      try {
        await this.updateOrderStatus({ orderId: order.order_id, status: 'delivered' });
        this.$emit('snackbar', { message: `订单 #${order.order_id} 已确认送达`, color: 'success' });
      } catch (error) {
        console.error('确认送达失败', error);
        this.$emit('snackbar', { message: '确认送达失败，请稍后再试', color: 'error' });
      }
    },

    // 初始化方法
    async initialize() {
      this.loading = true;
      try {
        await this.fetchAllOrders();  // 调用 Vuex 中的 action 获取所有订单
        console.log('当前用户 ID:', this.userId);
        console.log('所有订单:', this.allOrders);

        // 检查每个订单的 delivery_person.delivery_id
        this.allOrders.forEach(order => {
          console.log(`订单 #${order.order_id} 的配送员 ID:`, order.delivery_person?.delivery_id);
        });
      } catch (error) {
        console.error('获取订单失败:', error);
        this.$emit('snackbar', { message: '获取订单失败，请稍后再试', color: 'error' });
      } finally {
        this.loading = false;
      }
    },
  },
  created() {
    this.initialize();
  },
  watch: {
    userId(newVal, oldVal) {
      if (newVal !== oldVal) {
        this.initialize();
      }
    },
  },
};
</script>

<style scoped>
/* 添加您需要的样式 */
</style>
