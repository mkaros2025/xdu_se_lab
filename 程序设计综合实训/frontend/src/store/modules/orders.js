// src/store/modules/orders.js

import apiClient from '@/services/api'

const CART_STORAGE_KEY = 'user_cart' // 定义 localStorage 的 key

const state = {
  orders: [], // 当前订单（顾客）
  cart: JSON.parse(localStorage.getItem(CART_STORAGE_KEY)) || [], // 初始化购物车
  historicalOrders: [], // 历史订单（顾客）
  allOrders: [], // 商家的订单列表
}

const mutations = {
  // 顾客相关 mutations
  SET_ORDERS(state, orders) {
    state.orders = orders
  },
  SET_HISTORICAL_ORDERS(state, orders) {
    state.historicalOrders = orders
  },
  ADD_TO_CART(state, item) {
    const existingItem = state.cart.find(cartItem => cartItem.item_id === item.item_id)
    if (existingItem) {
      existingItem.quantity += item.quantity
    } else {
      state.cart.push({ ...item, quantity: item.quantity || 1 })
    }
    localStorage.setItem(CART_STORAGE_KEY, JSON.stringify(state.cart)) // 持久化到 localStorage
  },
  REMOVE_FROM_CART(state, itemId) {
    state.cart = state.cart.filter(item => item.item_id !== itemId)
    localStorage.setItem(CART_STORAGE_KEY, JSON.stringify(state.cart)) // 持久化到 localStorage
  },
  CLEAR_CART(state) {
    state.cart = []
    localStorage.removeItem(CART_STORAGE_KEY) // 清除 localStorage 中的购物车数据
  },

  // 商家相关 mutations
  SET_ALL_ORDERS(state, orders) {
    state.allOrders = orders
      .filter(order => order !== null) // 过滤掉 null 项
      .map(order => ({
        ...order,
        total_price: Number(order.total_price), // 确保 total_price 是数字
        order_time: new Date(order.order_time), // 将订单时间转换为 Date 对象
        order_items: order.order_items.map(item => ({
          ...item,
          price: Number(item.price), // 确保每个订单项的 price 是数字
        })),
        // 保留 delivery_person 字段
        has_review: order.has_review || false, // 标记订单是否已评价
      }))
    console.log('SET_ALL_ORDERS:', state.allOrders) // 调试日志
  },
  UPDATE_ORDER_STATUS(state, { orderId, status }) {
    const order = state.allOrders.find(order => order.order_id === orderId)
    if (order) {
      order.status = status
    }
  },

  // 新增 mutation: 标记订单已评价
  SET_ORDER_REVIEWED(state, orderId) {
    const order = state.historicalOrders.find(order => order.order_id === orderId)
    if (order) {
      order.has_review = true
    }
  },
}

const actions = {
  // 顾客相关 actions

  // 创建新订单
  async createOrder({ commit, state, dispatch }, orderData) {
    try {
      const response = await apiClient.post('/orders', orderData)
      // 添加到当前订单列表
      commit('SET_ORDERS', [...state.orders, response.data])
      // 清空购物车
      commit('CLEAR_CART')
      // 重新获取历史订单
      await dispatch('fetchHistoricalOrders')
    } catch (error) {
      console.error('创建订单失败：', error)
      throw error
    }
  },

  // 添加商品到购物车
  addToCart({ commit }, item) {
    commit('ADD_TO_CART', item)
  },

  // 从购物车移除商品
  removeFromCart({ commit }, itemId) {
    commit('REMOVE_FROM_CART', itemId)
  },

  // 清空购物车
  clearCart({ commit }) {
    commit('CLEAR_CART')
  },

  // 获取历史订单
  async fetchHistoricalOrders({ commit, rootGetters }) {
    try {
      const userId = rootGetters['auth/userId'] // 获取当前用户的 user_id
      if (!userId) {
        throw new Error('用户未登录')
      }
      const response = await apiClient.get(`/users/${userId}/orders`)
      // 假设后端返回的数据包含订单列表
      const orders = response.data.map(order => ({
        ...order,
        total_price: Number(order.total_price),
        order_time: new Date(order.order_time),
        has_review: false, // 初始化为未评价
      }))

      // 为每个订单查询是否有评价
      const reviewPromises = orders.map(async (order) => {
        try {
          const reviewResponse = await apiClient.get('/reviews', {
            params: { order_id: order.order_id },
          })
          console.log(`查询订单 ${order.order_id} 的评价结果:`, reviewResponse.data)
          if (Array.isArray(reviewResponse.data) && reviewResponse.data.length > 0) {
            order.has_review = true
          } else {
            order.has_review = false
          }
        } catch (error) {
          // 假设返回错误表示没有评价
          if (error.response && error.response.data && error.response.data.message) {
            order.has_review = false
          } else {
            // 处理其他错误
            console.error(`查询订单 ${order.order_id} 的评价失败：`, error)
            order.has_review = false
          }
        }
      })

      // 等待所有评价查询完成
      await Promise.all(reviewPromises)

      // 提交设置历史订单
      commit('SET_HISTORICAL_ORDERS', orders)
      console.log('SET_HISTORICAL_ORDERS 已提交:', orders)
    } catch (error) {
      console.error('获取历史订单失败：', error)
      throw error
    }
  },

  // 商家相关 actions

  // 获取商家的所有订单
  async fetchAllOrders({ commit, rootGetters }) { // 添加 rootGetters
    try {
      const userId = rootGetters['auth/userId']
      const response = await apiClient.get('/orders') // GET请求到 /api/orders
      // 如果需要根据用户类型或ID进行过滤，可在这里处理
      commit('SET_ALL_ORDERS', response.data)
    } catch (error) {
      console.error('获取所有订单失败：', error)
      throw error
    }
  },

  // 商家接受订单或出餐
  async updateOrderStatus({ dispatch, commit }, { orderId, status }) {
    try {
      await apiClient.put(`/orders/${orderId}/status`, { status })
      // 重新获取所有订单以更新状态
      await dispatch('fetchAllOrders')
      // 或者，使用 mutation 直接更新状态
      // commit('UPDATE_ORDER_STATUS', { orderId, status })
    } catch (error) {
      console.error(`更新订单 ${orderId} 状态失败：`, error)
      throw error
    }
  },

  // 商家请求送单（如果需要）
  async requestDelivery({ commit }, orderId) {
    try {
      const response = await apiClient.post(`/orders/${orderId}/request_delivery`)
      if (response.status === 200) {
        commit('UPDATE_ORDER_STATUS', { orderId, status: 'delivering' }) // 假设状态改为 'delivering'
      }
    } catch (error) {
      console.error('请求送单失败：', error)
      throw error
    }
  },

  // 配送员相关 actions
  async updateOrderDeliveryPerson({ dispatch }, { orderId, deliveryPersonId }) {
    try {
      // 向 API 发送请求更新订单的配送员信息
      await apiClient.put(`/orders/${orderId}/delivery_person`, { delivery_person_id: deliveryPersonId });
      // 获取所有订单并更新配送员信息
      await dispatch('fetchAllOrders');
    } catch (error) {
      console.error('更新订单配送员失败', error);
      throw error;
    }
  },

  // 添加评价
  async addReview({ commit }, reviewData) {
    try {
      const { order_id, customer_id, rating, comment } = reviewData

      // 发送评价数据到后端
      const response = await apiClient.post('/reviews', {
        order_id,
        customer_id,
        rating,
        comment,
      })

      console.log(`订单 ${order_id} 评价提交成功:`, response.data)

      // 标记订单为已评价
      commit('SET_ORDER_REVIEWED', order_id)

      return response.data
    } catch (error) {
      console.error('添加评价失败：', error)
      throw error
    }
  },
}

const getters = {
  // 顾客相关 getters
  customerOrders: (state) => state.orders,
  cartItems: (state) => state.cart,
  historicalOrders: (state) => state.historicalOrders, // 修复语法错误
  totalPrice: (state) =>
    state.cart.reduce((total, item) => total + parseFloat(item.price) * item.quantity, 0),

  // 商家相关 getters
  merchantAllOrders: (state) => state.allOrders,

  // 添加一个别名 getter
  allOrders: (state) => state.allOrders,
}

export default {
  namespaced: true,
  state,
  mutations,
  actions,
  getters,
}
