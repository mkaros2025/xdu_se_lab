<!-- src/views/ManageOrders.vue -->
<template>
  <v-container>
    <h1 class="mb-5">管理所有订单</h1>
    
    <!-- 订单表格 -->
    <v-card>
      <v-card-title class="headline">所有订单</v-card-title>
      <v-card-text>
        <div v-if="isLoading" class="text-center my-5">
          <v-progress-circular indeterminate color="primary"></v-progress-circular>
        </div>
        <div v-else>
          <v-data-table
            :headers="tableHeaders"
            :items="allOrders"
            :items-per-page="10"
            class="elevation-1"
            :loading="isLoading"
            loading-text="正在加载订单..."
            item-key="order_id"
          >
            <!-- 自定义表格内容 -->
            <template v-slot:item.order_id="{ item }">
              {{ item.order_id }}
            </template>
            <template v-slot:item.customer_name="{ item }">
              {{ item.customer_name }}
            </template>
            <template v-slot:item.customer_phone="{ item }">
              {{ item.customer_phone }}
            </template>
            <template v-slot:item.delivery_address="{ item }">
              {{ item.delivery_address }}
            </template>
            <template v-slot:item.order_items="{ item }">
              <v-list dense>
                <v-list-item v-for="(orderItem, index) in item.order_items" :key="index">
                  <v-list-item-content>
                    <v-list-item-title>{{ orderItem.dish_name }}</v-list-item-title>
                    <v-list-item-subtitle>
                      数量: {{ orderItem.quantity }} | 价格: ¥{{ orderItem.price.toFixed(2) }}
                    </v-list-item-subtitle>
                  </v-list-item-content>
                </v-list-item>
              </v-list>
            </template>
            <template v-slot:item.total_price="{ item }">
              ¥{{ item.total_price.toFixed(2) }}
            </template>
            <template v-slot:item.status="{ item }">
              <v-chip :color="getStatusColor(item.status)" dark>
                {{ statusText(item.status) }}
              </v-chip>
            </template>
            <template v-slot:item.order_time="{ item }">
              {{ formatDate(item.order_time) }}
            </template>
            <template v-slot:item.actions="{ item }">
              <v-btn
                v-if="item.status === 'pending'"
                color="blue"
                small
                @click="acceptOrder(item.order_id)"
              >
                接受订单
              </v-btn>
              <v-btn
                v-if="item.status === 'preparing'"
                color="green"
                small
                @click="startDelivering(item.order_id)"
              >
                出餐
              </v-btn>
            </template>
          </v-data-table>
          
          <!-- 无订单提示 -->
          <div v-if="allOrders.length === 0" class="text-center mt-5">
            <v-icon large color="grey">mdi-emoticon-sad-outline</v-icon>
            <p>暂无订单。</p>
          </div>
        </div>
      </v-card-text>
    </v-card>
    
    <!-- 成功与错误消息 -->
    <v-snackbar v-model="snackbar.show" :color="snackbar.color" timeout="6000">
      {{ snackbar.message }}
      <v-btn color="white" text @click="snackbar.show = false">关闭</v-btn>
    </v-snackbar>
  </v-container>
</template>

<script>
import { mapGetters, mapActions } from 'vuex'

export default {
  name: 'ManageOrders',
  data() {
    return {
      isLoading: false, // 加载状态
      snackbar: {
        show: false,
        message: '',
        color: '',
      },
      tableHeaders: [
        { text: '订单编号', value: 'order_id', align: 'center'},
        { text: '客户姓名', value: 'customer_name' },
        { text: '客户电话', value: 'customer_phone' },
        { text: '配送地址', value: 'delivery_address' },
        { text: '菜品信息', value: 'order_items' },
        { text: '总价', value: 'total_price' },
        { text: '状态', value: 'status' },
        { text: '下单时间', value: 'order_time' },
        { text: '操作', value: 'actions', sortable: false },
      ],
    }
  },
  computed: {
    ...mapGetters('orders', ['allOrders']),
  },
  methods: {
    ...mapActions('orders', ['fetchAllOrders', 'updateOrderStatus']),
    
    // 格式化日期
    formatDate(date) {
      if (!(date instanceof Date)) {
        date = new Date(date)
      }
      const options = {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
      }
      return date.toLocaleString(undefined, options)
    },
    
    // 获取状态颜色
    getStatusColor(status) {
      switch (status) {
        case 'pending':
          return 'grey'
        case 'preparing':
          return 'blue'
        case 'predelivering':
          return 'orange'
        case 'delivering':
          return 'purple'
        case 'delivered':
          return 'green'
        case 'cancelled':
          return 'red'
        default:
          return 'grey'
      }
    },
    
    // 显示更友好的状态文本
    statusText(status) {
      const statusMap = {
        pending: '待接受',
        preparing: '准备中',
        predelivering: '已出餐', // 将 'predelivering' 映射为 '已出餐'
        delivering: '配送中',
        delivered: '已完成',
        cancelled: '已取消',
      }
      return statusMap[status] || status
    },
    
    // 接受订单
    async acceptOrder(orderId) {
      try {
        await this.updateOrderStatus({ orderId, status: 'preparing' })
        this.snackbar.message = `订单 #${orderId} 已接受。`
        this.snackbar.color = 'success'
        this.snackbar.show = true
      } catch (error) {
        this.snackbar.message = `接受订单 #${orderId} 失败。`
        this.snackbar.color = 'error'
        this.snackbar.show = true
      }
    },
    
    // 出餐
    async startDelivering(orderId) {
      try {
        await this.updateOrderStatus({ orderId, status: 'predelivering' }) // 更新状态为 'predelivering'
        this.snackbar.message = `订单 #${orderId} 已开始出餐。`
        this.snackbar.color = 'success'
        this.snackbar.show = true
      } catch (error) {
        this.snackbar.message = `出餐订单 #${orderId} 失败。`
        this.snackbar.color = 'error'
        this.snackbar.show = true
      }
    },
    
    // 加载所有订单
    async loadOrders() {
      this.isLoading = true
      try {
        await this.fetchAllOrders()
      } catch (error) {
        this.snackbar.message = '加载订单失败，请稍后再试。'
        this.snackbar.color = 'error'
        this.snackbar.show = true
        console.error('加载订单失败：', error)
      } finally {
        this.isLoading = false
      }
    },
  },
  watch: {
    allOrders(newOrders) {
      newOrders.forEach(order => {
        order.order_items.forEach(item => {
          console.log(`Order ID: ${order.order_id}, Dish: ${item.dish_name}, Price: ${item.price} (Type: ${typeof item.price})`)
        })
      })
    }
  },
  created() {
    this.loadOrders()
  },
}
</script>

<style scoped>
.mb-5 {
  margin-bottom: 2.5rem;
}
.mb-8 {
  margin-bottom: 4rem;
}
.my-5 {
  margin-top: 2rem;
  margin-bottom: 2rem;
}
.mt-5 {
  margin-top: 2rem;
}
.text-center {
  text-align: center;
}

.rounded-order-panel {
  border-radius: 16px !important;
  overflow: hidden;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.v-data-table {
  border-radius: 12px;
}

.v-data-table thead {
  background-color: #e0e0e0;
}

.v-data-table th {
  padding: 12px 16px;
}

.v-data-table td {
  padding: 8px 16px;
}

.v-chip {
  font-weight: bold;
}

@media (max-width: 600px) {
  .v-data-table th,
  .v-data-table td {
    padding: 8px 12px;
  }
}
</style>
