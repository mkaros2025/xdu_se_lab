<!-- src/views/OrdersPage.vue -->
<template>
  <v-container>
    <h1 class="mb-5">我的订单</h1>

    <!-- 购物车部分 -->
    <v-card class="mb-8">
      <v-card-title class="headline">购物车结算</v-card-title>
      <v-card-text>
        <v-list>
          <v-list-item v-for="cartItem in cart" :key="cartItem.item_id" class="py-2">
            <v-row class="align-center">
              <!-- 商品图片 -->
              <v-col cols="auto">
                <div class="circle-image" :style="{ backgroundImage: `url(${cartItem.image_url || defaultImage})` }">
                </div>
              </v-col>

              <!-- 商品详情 -->
              <v-col>
                <v-list-item-title class="font-weight-bold">
                  {{ cartItem.name }}
                </v-list-item-title>
                <v-list-item-subtitle>
                  数量: {{ cartItem.quantity }}
                </v-list-item-subtitle>
              </v-col>

              <!-- 操作按钮 -->
              <v-col cols="auto" class="text-right">
                <v-btn icon @click="removeFromCart(cartItem.item_id)">
                  <v-icon color="red">mdi-delete</v-icon>
                </v-btn>
              </v-col>
            </v-row>
          </v-list-item>
        </v-list>

        <v-divider class="my-4"></v-divider>
        <v-row justify="end">
          <v-col cols="12" class="text-right">
            <h3 class="mb-3">总价: ¥{{ totalPrice.toFixed(2) }}</h3>
            <v-form ref="orderForm" v-model="valid" class="mt-3">
              <!-- 配送地址 -->
              <v-text-field v-model="deliveryAddress" label="配送地址" :rules="[v => !!v || '配送地址为必填项']" required outlined
                dense></v-text-field>

              <!-- 配送时间 -->
              <v-text-field v-model="deliveryTime" label="配送时间" type="time" :rules="[v => !!v || '配送时间为必填项']" required
                outlined dense></v-text-field>
            </v-form>
            <v-btn color="success" @click="submitOrder" :disabled="!valid || cart.length === 0" class="mt-4">
              提交订单
            </v-btn>
          </v-col>
        </v-row>
      </v-card-text>
    </v-card>

    <!-- 历史订单部分 -->
    <v-card>
      <v-card-title class="headline">历史订单</v-card-title>
      <v-card-text>
        <div v-if="isLoading" class="text-center my-5">
          <v-progress-circular indeterminate color="primary"></v-progress-circular>
        </div>
        <div v-else>
          <v-expansion-panels multiple>
            <v-expansion-panel v-for="order in sortedHistoricalOrders" :key="order.order_id"
              class="rounded-order-panel mb-4">
              <v-expansion-panel-header class="rounded-order-header">
                <v-row class="w-100">
                  <v-col cols="12" md="8">
                    <strong>订单 #{{ order.order_id }}</strong>
                    <span class="grey--text">- {{ formatDate(order.order_time) }}</span>
                  </v-col>
                  <v-col cols="12" md="4" class="text-md-right mt-2 mt-md-0">
                    <v-chip :color="getStatusColor(order.status)" dark>
                      {{ getStatusText(order.status) }}
                    </v-chip>
                  </v-col>
                </v-row>
              </v-expansion-panel-header>
              <v-expansion-panel-content>
                <v-card class="order-content-card rounded-order-content">
                  <v-data-table :headers="orderTableHeaders" :items="order.order_items" class="elevation-1"
                    hide-default-footer dense>
                    <!-- 自定义表头 -->
                    <template v-slot:header.dish_name>
                      <span class="font-weight-bold">菜品名称</span>
                    </template>
                    <template v-slot:header.quantity>
                      <span class="font-weight-bold">数量</span>
                    </template>
                    <template v-slot:header.price>
                      <span class="font-weight-bold">价格</span>
                    </template>

                    <!-- 自定义表格内容 -->
                    <template v-slot:item.dish_name="{ item }">
                      {{ item.dish_name }}
                    </template>
                    <template v-slot:item.quantity="{ item }">
                      {{ item.quantity }}
                    </template>
                    <template v-slot:item.price="{ item }">
                      ¥{{ (Number(item.price) * item.quantity).toFixed(2) }}
                    </template>
                  </v-data-table>
                  <!-- 新增的布局部分 -->
                  <v-row class="mt-1" align="center" justify="space-between">
                    <v-col cols="12" md="6">
                      <v-btn color="primary" @click="openReviewDialog(order)"
                        :disabled="order.status !== 'delivered' || order.has_review">
                        评价
                      </v-btn>
                      <span v-if="order.has_review" class="ml-3 text-success">已评价</span>
                    </v-col>
                    <v-col cols="12" md="6" class="text-md-right mt-2 mt-md-0">
                      <h4 class="mb-0">订单总价: ¥{{ Number(order.total_price).toFixed(2) }}</h4>
                    </v-col>
                  </v-row>
                </v-card>
              </v-expansion-panel-content>
            </v-expansion-panel>
          </v-expansion-panels>
          <div v-if="sortedHistoricalOrders.length === 0" class="text-center mt-5">
            <v-icon large color="grey">mdi-emoticon-sad-outline</v-icon>
            <p>暂无历史订单。</p>
          </div>
        </div>
      </v-card-text>
    </v-card>

    <!-- 评价弹窗 -->
    <v-dialog v-model="reviewDialog.show" max-width="500px">
      <v-card>
        <v-card-title class="headline">添加评价</v-card-title>
        <v-card-text>
          <v-form ref="reviewForm" v-model="reviewValid">
            <v-text-field v-model="review.rating" label="评分 (1 ~ 5)" type="number" :rules="ratingRules" required
              outlined dense></v-text-field>
            <v-textarea v-model="review.comment" label="评论" :rules="[v => !!v || '评论为必填项']" required outlined
              dense></v-textarea>
          </v-form>
        </v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text @click="closeReviewDialog">取消</v-btn>
          <v-btn color="success" @click="submitReview" :disabled="!reviewValid">
            提交
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>

    <!-- 成功与错误消息 -->
    <v-snackbar v-model="snackbar.show" :color="snackbar.color" timeout="6000">
      {{ snackbar.message }}
      <v-btn color="white" text @click="snackbar.show = false">关闭</v-btn>
    </v-snackbar>
  </v-container>
</template>

<script>
/* eslint-disable */
import { mapGetters, mapActions } from 'vuex'

export default {
  name: 'OrdersPage',
  data() {
    return {
      deliveryAddress: '',
      deliveryTime: '',
      valid: false,
      isLoading: false, // 加载状态
      snackbar: {
        show: false,
        message: '',
        color: '',
      },
      defaultImage: 'https://via.placeholder.com/50', // 默认图片
      orderTableHeaders: [
        { text: '菜品名称', value: 'dish_name' },
        { text: '数量', value: 'quantity' },
        { text: '价格', value: 'price' },
      ],
      // 评价弹窗相关数据
      reviewDialog: {
        show: false,
      },
      review: {
        order_id: null,
        rating: null,
        comment: '',
      },
      reviewValid: false,
      // 评分验证规则
      ratingRules: [
        v => (v === null || v === undefined || v === '') ? '评分为必填项' : true,
        v => Number.isInteger(Number(v)) || '评分必须是整数',
        v => (v >= 1 && v <= 5) || '评分必须在1到5之间',
      ],
    }
  },
  computed: {
    ...mapGetters('orders', ['cartItems', 'totalPrice', 'historicalOrders']),
    cart() {
      return this.cartItems
    },
    user() {
      return this.$store.getters['auth/getUser']
    },
    sortedHistoricalOrders() {
      // 复制 historicalOrders 数组并按 order_time 降序排序
      return [...this.historicalOrders].sort((a, b) => new Date(b.order_time) - new Date(a.order_time))
    },
  },
  methods: {
    ...mapActions('orders', ['removeFromCart', 'createOrder', 'clearCart', 'fetchHistoricalOrders', 'addReview']),

    // 提交订单
    async submitOrder() {
      if (!this.$refs.orderForm.validate()) {
        return
      }

      try {
        if (!this.user) {
          throw new Error('用户未登录')
        }

        // 确保配送时间已选择
        if (!this.deliveryTime) {
          this.snackbar.message = '请选择配送时间。'
          this.snackbar.color = 'error'
          this.snackbar.show = true
          return
        }

        // 获取当前日期
        const today = new Date()
        const year = today.getFullYear()
        const month = String(today.getMonth() + 1).padStart(2, '0')
        const day = String(today.getDate()).padStart(2, '0')

        // 格式化时间，确保有秒数
        const [hours, minutes] = this.deliveryTime.split(':')
        const combinedDeliveryTime = `${year}-${month}-${day} ${hours}:${minutes}:00`

        const orderData = {
          customer_id: this.user.user_id, // 确保包含 customer_id
          delivery_time: combinedDeliveryTime,
          delivery_address: this.deliveryAddress,
          items: this.cart.map(item => ({
            item_id: item.item_id,
            quantity: item.quantity,
          })),
        }
        await this.createOrder(orderData)
        this.snackbar.message = '订单已提交成功！'
        this.snackbar.color = 'success'
        this.snackbar.show = true
        this.deliveryAddress = ''
        this.deliveryTime = ''
      } catch (error) {
        this.snackbar.message = '订单提交失败，请重试。'
        this.snackbar.color = 'error'
        this.snackbar.show = true
        console.error('订单提交失败：', error)
      }
    },

    // 加载历史订单并检查是否已评价
    async loadHistoricalOrders() {
      this.isLoading = true
      try {
        await this.fetchHistoricalOrders()
      } catch (error) {
        this.snackbar.message = '加载历史订单失败，请稍后再试。'
        this.snackbar.color = 'error'
        this.snackbar.show = true
        console.error('加载历史订单失败：', error)
      } finally {
        this.isLoading = false
      }
    },

    // 格式化日期
    formatDate(dateString) {
      const options = {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
      }
      return new Date(dateString).toLocaleString(undefined, options)
    },

    // 获取订单状态颜色
    getStatusColor(status) {
      switch (status) {
        case '已完成':
        case 'delivered': // 添加 'delivered' 的颜色
          return 'green'
        case '配送中':
        case 'delivering': // 添加 'delivering' 的颜色
          return 'blue'
        case '已取消':
        case 'cancelled': // 添加 'cancelled' 的颜色
          return 'red'
        default:
          return 'grey'
      }
    },

    // 获取订单状态文本
    getStatusText(status) {
      switch (status) {
        case 'delivered':
          return '已完成'
        case 'delivering':
          return '配送中'
        case 'cancelled':
          return '已取消'
        default:
          return status
      }
    },

    // 打开评价弹窗
    openReviewDialog(order) {
      this.review.order_id = order.order_id
      this.review.rating = null
      this.review.comment = ''
      this.reviewDialog.show = true
    },

    // 关闭评价弹窗
    closeReviewDialog() {
      this.reviewDialog.show = false
    },

    // 提交评价
    async submitReview() {
      if (!this.$refs.reviewForm.validate()) {
        return
      }

      try {
        const customer_id = this.user.user_id
        const rating = parseInt(this.review.rating, 10)

        // 再次验证 rating
        if (isNaN(rating) || rating < 1 || rating > 5) {
          this.snackbar.message = '评分必须是1到5之间的整数。'
          this.snackbar.color = 'error'
          this.snackbar.show = true
          return
        }

        await this.addReview({
          order_id: this.review.order_id,
          customer_id,
          rating,
          comment: this.review.comment.trim(),
        })
        this.snackbar.message = '评价已提交成功！'
        this.snackbar.color = 'success'
        this.snackbar.show = true
        this.closeReviewDialog()
        // 无需重新加载历史订单，因为 Vuex 已经标记了订单为已评价
      } catch (error) {
        this.snackbar.message = error.response?.data?.message || '评价提交失败，请重试。'
        this.snackbar.color = 'error'
        this.snackbar.show = true
        console.error('评价提交失败：', error)
      }
    },
  },
  created() {
    this.loadHistoricalOrders()
  },
}
</script>

<style scoped>
.circle-image {
  width: 50px; /* 圆形框的宽度 */
  height: 50px; /* 圆形框的高度 */
  border-radius: 50%; /* 确保显示为圆形 */
  overflow: hidden; /* 避免图片溢出 */
  background-size: cover; /* 确保图片填满框，并裁剪多余部分 */
  background-position: center; /* 图片居中显示 */
  background-repeat: no-repeat; /* 防止图片重复 */
  border: 1px solid #ddd; /* 可选：边框修饰 */
}

.circle-content {
  width: 100%;
  /* 自动适应宽度 */
  height: 100%;
  /* 高度充满框 */
  object-fit: fill;
  /* 填充内容并裁剪 */
}

.mb-5 {
  margin-bottom: 2.5rem;
}

.mb-8 {
  margin-bottom: 4rem;
}

.mt-3 {
  margin-top: 1rem;
}

.mt-4 {
  margin-top: 1.5rem;
}

.mt-5 {
  margin-top: 2rem;
}

.my-4 {
  margin-top: 1rem;
  margin-bottom: 1rem;
}

.mb-4 {
  margin-bottom: 1.5rem;
}

.text-right {
  text-align: right;
}

.text-md-right {
  text-align: right;
}

.ml-2 {
  margin-left: 0.5rem;
}

/* 自定义圆角样式 */
.rounded-order-panel {
  border-radius: 16px !important;
  overflow: hidden;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.rounded-order-header {
  background-color: #f5f5f5;
  border-top-left-radius: 16px;
  border-top-right-radius: 16px;
  padding: 16px;
}

.rounded-order-content {
  padding: 16px;
  border-bottom-left-radius: 16px;
  border-bottom-right-radius: 16px;
}

.order-content-card {
  background-color: #ffffff;
}

/* 优化表格样式 */
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

/* 响应式调整 */
@media (max-width: 600px) {

  .rounded-order-header,
  .rounded-order-content {
    padding: 12px;
  }

  .v-data-table th,
  .v-data-table td {
    padding: 8px 12px;
  }

  /* 新增的响应式样式 */
  .order-footer-row {
    flex-direction: column;
    align-items: flex-start;
  }

  .order-footer-row .text-md-right {
    text-align: left !important;
    margin-top: 8px;
  }
}
</style>
