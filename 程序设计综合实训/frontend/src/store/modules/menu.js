// src/store/modules/menu.js

import apiClient from '@/services/api'

const state = {
  menuItems: [],
}

const mutations = {
  SET_MENU_ITEMS(state, items) {
    state.menuItems = items
  },
  ADD_MENU_ITEM(state, item) {
    state.menuItems.push(item)
  },
  UPDATE_MENU_ITEM(state, updatedItem) {
    const index = state.menuItems.findIndex(item => item.item_id === updatedItem.item_id)
    if (index !== -1) {
      // 使用 splice 保持响应式
      state.menuItems.splice(index, 1, updatedItem)
    }
  },
  DELETE_MENU_ITEM(state, itemId) {
    state.menuItems = state.menuItems.filter(item => item.item_id !== itemId)
  },
}

const actions = {
  // 获取菜单
  async fetchMenu({ commit }) {
    try {
      const response = await apiClient.get('/menu') // GET请求到 /api/menu
      if (Array.isArray(response.data)) {
        commit('SET_MENU_ITEMS', response.data)
      } else {
        throw new Error('菜单数据格式不正确')
      }
    } catch (error) {
      console.error('获取菜单失败：', error)
      throw error
    }
  },

  // 添加菜单项
  async addMenuItem({ commit }, menuItem) {
    try {
      const response = await apiClient.post('/menu', menuItem) // POST请求到 /api/menu
      commit('ADD_MENU_ITEM', response.data)
    } catch (error) {
      console.error('添加菜单项失败：', error)
      throw error
    }
  },

  // 更新菜单项
  async updateMenuItem({ commit }, menuItem) {
    try {
      const response = await apiClient.put(`/menu/${menuItem.item_id}`, menuItem) // PUT请求到 /api/menu/<item_id>
      commit('UPDATE_MENU_ITEM', response.data)
    } catch (error) {
      console.error('更新菜单项失败：', error)
      throw error
    }
  },

  // 删除菜单项
  async deleteMenuItem({ commit }, itemId) {
    try {
      await apiClient.delete(`/menu/${itemId}`) // DELETE请求到 /api/menu/<item_id>
      commit('DELETE_MENU_ITEM', itemId)
    } catch (error) {
      console.error('删除菜单项失败：', error)
      throw error
    }
  },
}

const getters = {
  getMenuItems: (state) => state.menuItems,
}

export default {
  namespaced: true,
  state,
  mutations,
  actions,
  getters,
}
