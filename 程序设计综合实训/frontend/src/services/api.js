// src/services/api.js

import axios from 'axios'

// 创建 Axios 实例
const apiClient = axios.create({
  baseURL: 'http://localhost:5000/api', // 确保 baseURL 包含 /api
  headers: {
    'Content-Type': 'application/json',
  },
})

// 移除 Authorization 相关的拦截器，因为您不使用 JWT
/*
apiClient.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('token')
    if (token) {
      config.headers.Authorization = `Bearer ${token}`
    }
    return config
  },
  (error) => {
    return Promise.reject(error)
  }
)
*/

export default apiClient
