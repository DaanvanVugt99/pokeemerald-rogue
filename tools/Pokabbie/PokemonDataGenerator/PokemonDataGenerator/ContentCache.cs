using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator
{
	public static class ContentCache
	{
		public static readonly string c_CacheFolder = Path.GetFullPath("content_cache");
		public static readonly string c_ResourcesFolder = Path.GetFullPath("../../Resources");
		public static bool BypassHttpCache { get; set; }

		private static readonly SortedDictionary<string, string> s_FetchedContentHashes = new SortedDictionary<string, string>();
		private static readonly ConcurrentDictionary<string, string> s_FreshHttpContent = new ConcurrentDictionary<string, string>();
		private static readonly object s_HashLock = new object();

		public static void ResetFetchedContentTracking()
		{
			lock (s_HashLock)
				s_FetchedContentHashes.Clear();
			s_FreshHttpContent.Clear();
		}

		private static string UriToCachePath(string uri)
		{
			if (uri.StartsWith(c_CacheFolder, StringComparison.CurrentCultureIgnoreCase))
			{
				return uri.ToLower();
			}
			else if (uri.StartsWith("res://", StringComparison.CurrentCultureIgnoreCase))
			{
				return uri.Replace("res://", c_ResourcesFolder + "/").ToLower();
			}
			else
			{
				string keyName = "F_" + Path.GetFileName(uri).ToLower()
					.Replace("://", "__")
					.Replace("?", "Q")
					.Replace("=", "E")
					.Replace("c", "c")
					.Replace(":", "S")
					.Replace(";", "S");

				string basePath = Path.GetDirectoryName(uri).ToLower()
					.Replace("://", "__")
					.Replace("?", "Q")
					.Replace("=", "E")
					.Replace("c", "c")
					.Replace(":", "S")
					.Replace(";", "S");

				string path = Path.Combine(c_CacheFolder, basePath, keyName);

				string baseDir = Path.GetDirectoryName(path);
				Directory.CreateDirectory(baseDir);

				return path;
			}
		}

		public static string GetWriteableCachePath(string path)
		{
			return UriToCachePath(path);
		}

		public static bool ExistsInCache(string uri)
		{
			string cachePath = UriToCachePath(uri);
			return File.Exists(cachePath);
		}

		public static string GetHttpContent(string uri)
		{
			string cachePath = UriToCachePath(uri);
			if (!BypassHttpCache && File.Exists(cachePath))
				return File.ReadAllText(cachePath);
			if (BypassHttpCache && s_FreshHttpContent.TryGetValue(uri, out string freshContent))
				return freshContent;

			using (HttpClient web = new HttpClient())
			{
				var task = web.GetStringAsync(uri);
				task.Wait();

				if (BypassHttpCache)
				{
					s_FreshHttpContent[uri] = task.Result;
					lock (s_HashLock)
						s_FetchedContentHashes[uri] = ComputeSha256(task.Result);
				}
				else
					File.WriteAllText(cachePath, task.Result);
				return task.Result;
			}
		}

		public static void PrefetchHttpContent(IEnumerable<string> uris)
		{
			Parallel.ForEach(
				uris.Distinct(),
				new ParallelOptions { MaxDegreeOfParallelism = 12 },
				uri => GetHttpContent(uri));
		}

		public static string GetFetchedContentAggregateHash()
		{
			StringBuilder content = new StringBuilder();
			foreach (var entry in s_FetchedContentHashes.OrderBy(entry => entry.Key))
				content.Append(entry.Key).Append('\n').Append(entry.Value).Append('\n');
			return ComputeSha256(content.ToString());
		}

		private static string ComputeSha256(string content)
		{
			using (SHA256 sha = SHA256.Create())
			{
				byte[] hash = sha.ComputeHash(Encoding.UTF8.GetBytes(content));
				return string.Concat(hash.Select(value => value.ToString("x2")));
			}
		}

		public static Bitmap GetImageContent(string uri)
		{
			string cachePath = UriToCachePath(uri);
			if (File.Exists(cachePath))
				return new Bitmap(cachePath);

			WebRequest request = WebRequest.Create(uri);
			Bitmap result = new Bitmap(request.GetResponse().GetResponseStream());
			result.Save(cachePath);
			return result;
		}

		public static JObject GetJsonContent(string uri)
		{
			string content = GetHttpContent(uri);
			return JObject.Parse(content);
		}
	}
}
